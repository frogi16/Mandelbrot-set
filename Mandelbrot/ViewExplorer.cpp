#include "ViewExplorer.h"
#include "Mandelbrot.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

ViewExplorer::ViewExplorer(sf::IntRect dimen, Mandelbrot& own) :
	dimensions(dimen),
	columns(3),
	owner(own)
{
	arial.loadFromFile("arial.ttf");
	explorerTexture.create(dimensions.width, dimensions.height);
	explorerSprite.setPosition(dimensions.left, dimensions.top);

	reloadButton = std::make_shared<Button>(sf::IntRect(dimensions.left + dimensions.width - 90, 10, 80, 40), "Reload");
	buttons.push_back(reloadButton);

	loadViews();
	orderSprites();
}

void ViewExplorer::handleMouse(const sf::Mouse &mouse)
{
	//this class draws all of its components on RenderTexture using texture's local coordinates and for this reason proper handling of mouse events requires translating mouse coordinates to local systen

	sf::Vector2i localMouse = toLocalCoordinates(mouse.getPosition() + sf::Vector2i{ 0, scrollPosition });

	for (auto& i : buttons)
	{
		i->updateMouse(localMouse, mouse.isButtonPressed(sf::Mouse::Button::Left));
	}

	bool grabbed = false;
	for (auto& i : represesentations)
	{
		if (i.dimensions.contains(localMouse.x, localMouse.y) && mouse.isButtonPressed(sf::Mouse::Button::Left))		//grab
		{
			grabbedRepresentation = &i;
			grabbed = true;
		}
		else if (i.dimensions.contains(localMouse.x, localMouse.y) && grabbedRepresentation == (&i) && !mouse.isButtonPressed(sf::Mouse::Button::Left))		//select
		{
			selectedRepresentation = &i;
			selectedFrame.setPosition(selectedRepresentation->dimensions.left, selectedRepresentation->dimensions.top);
			owner.changeCurrentView(selectedRepresentation->data, selectedRepresentation->previewTexture);
		}
	}
	if (!grabbed)
		grabbedRepresentation = nullptr;

	if (reloadButton->clicked())
	{
		loadViews();
		orderSprites();
	}

}

void ViewExplorer::scroll(const sf::Event &event)
{
	if (scrollable)
	{
		constexpr float coefficient = -30;

		scrollPosition += event.mouseWheelScroll.delta*coefficient;

		scrollPosition = (scrollPosition < 0) ? 0 : scrollPosition;		//checking range
		scrollPosition = (scrollPosition > realHeight - dimensions.height) ? realHeight - dimensions.height : scrollPosition;
	}
}

void ViewExplorer::draw(sf::RenderTarget & target)
{
	explorerTexture.clear(sf::Color(220, 220, 220));
	auto size = explorerTexture.getSize();

	if (selectedRepresentation)
		explorerTexture.draw(selectedFrame);

	for (auto& i : represesentations)
	{
		explorerTexture.draw(i.previewSprite);
		explorerTexture.draw(i.nameText);
	}

	for (auto& i : buttons)
	{
		i->draw(explorerTexture);
	}

	explorerTexture.display();
	explorerSprite.setTexture(explorerTexture.getTexture());

	if (scrollable)
	{
		explorerSprite.setTextureRect(sf::IntRect{ dimensions.left, scrollPosition, dimensions.width, dimensions.height });
	}

	target.draw(explorerSprite);
}

ViewExplorer::~ViewExplorer()
{
}

void ViewExplorer::loadViews()
{
	for (auto& i : std::experimental::filesystem::directory_iterator("."))		//dot means current directory. C++17 only, experimental approach, but I used old solutions and I'd rather avoid doing that again
	{
		if (i.path().extension() == ".txt")
		{
			std::experimental::filesystem::path current = i;

			std::string name = i.path().filename().generic_string();		//without extension
			name.erase(name.end() - 4, name.end());

			if (!previewExists(name))
			{
				loadViewData(i.path().filename().generic_string());		//using these long namespaces and types is tiring and one day "experimental" namespace will be removed, so I converted it to standard string at the first opportunity. If implementation changes it will require changes only in this function (I hope)

				if (std::experimental::filesystem::exists(current.replace_extension(".png")))
				{
					loadViewImage(current.filename().generic_string());
				}
			}
		}
	}
}

void ViewExplorer::loadViewData(std::string filename)
{
	std::ifstream in(filename);
	std::stringstream buffer;
	buffer << in.rdbuf();	//apparently the most efficient way and works even under C++98

	std::vector<std::string> values;
	while (!buffer.eof())
	{
		std::string temp;
		buffer >> temp;
		values.push_back(std::move(temp));
	}

	filename.erase(filename.end() - 4, filename.end());		//cut.png
	ViewRepresentation temp{ View{values}, filename };
	represesentations.push_back(std::move(temp));
}

void ViewExplorer::loadViewImage(std::string filename)
{
	std::string name = filename;
	name.erase(name.end() - 4, name.end());		//cut .png

	for (auto& i : represesentations)
	{
		if (i.name == name)		//found linked data
		{
			i.previewTexture.loadFromFile(filename);
		}
	}
}

void ViewExplorer::orderSprites()
{
	constexpr int margin = 30;		//every side
	constexpr float nameHeight = 40;
	int columnWidth = (dimensions.width - 2 * margin) / float(columns);
	int columnHeight = columnWidth + nameHeight;
	int previewWidth = columnWidth * 0.8f;
	int previewMargin = columnWidth * 0.1f;

	setSelectedFrameProperties(columnWidth, columnHeight);

	if (represesentations.size() > 0)
	{
		realHeight = margin * 2 + ((represesentations.size() - 1) / columns + 1)*columnHeight;
		scrollable = (realHeight > dimensions.height) ? true : false;

		if (scrollable)
		{
			explorerTexture.create(dimensions.width, realHeight);		//scale to new size
		}
	}

	for (size_t i = 0; i < represesentations.size(); i++)
	{
		float newScale = previewWidth / float(represesentations[i].previewTexture.getSize().x);
		int actualColumn = i % columns;
		int actualRow = i / columns;

		auto &sprite = represesentations[i].previewSprite;
		sprite.setTexture(represesentations[i].previewTexture);
		sprite.setScale(newScale, newScale);
		sprite.setPosition(margin + columnWidth*actualColumn + previewMargin, margin + columnHeight*actualRow + previewMargin);

		sf::FloatRect textArea{ sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + sprite.getGlobalBounds().height, sprite.getGlobalBounds().width, nameHeight };		//area which contains name of representation
		setNameTextProperties(represesentations[i].nameText, textArea, represesentations[i].name);

		represesentations[i].dimensions = sf::IntRect{ margin + columnWidth*actualColumn, margin + columnHeight*actualRow, columnWidth, columnHeight };
	}
}

void ViewExplorer::setNameTextProperties(sf::Text & nameText, const sf::FloatRect &area, const std::string &name)
{
	nameText.setFont(arial);
	nameText.setColor(sf::Color::Black);
	nameText.setCharacterSize(20);
	nameText.setString(name);
	nameText.setPosition(centerIn(nameText.getLocalBounds(), area));
}

void ViewExplorer::setSelectedFrameProperties(int width, int height)
{
	selectedFrame.setSize(sf::Vector2f(width, height));
	selectedFrame.setFillColor(sf::Color(200, 200, 200));
}

bool ViewExplorer::previewExists(const std::string & name)
{
	for (auto& i : represesentations)
	{
		if (i.name == name)
			return true;
	}
	return false;
}

sf::Vector2f ViewExplorer::centerIn(const sf::FloatRect &centeredObject, const sf::FloatRect &area) const
{
	return sf::Vector2f(area.left + (area.width - centeredObject.width) / 2, area.top + (area.height - centeredObject.height) / 2);
}
