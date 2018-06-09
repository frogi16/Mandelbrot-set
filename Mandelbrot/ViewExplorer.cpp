#include "ViewExplorer.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

ViewExplorer::ViewExplorer(sf::IntRect dimen) : dimensions(dimen), columns(3)
{
	arial.loadFromFile("arial.ttf");
	explorerTexture.create(dimensions.width, dimensions.height);
	explorerSprite.setPosition(dimensions.left, dimensions.top);
	loadViews();
	orderSprites();
}

void ViewExplorer::handleMouse(sf::Mouse mouse)
{
}

void ViewExplorer::draw(sf::RenderTarget & target)
{
	explorerTexture.clear(sf::Color(220, 220, 220));
	auto size = explorerTexture.getSize();

	for (auto& i : represesentations)
	{
		explorerTexture.draw(i.previewSprite);
		explorerTexture.draw(i.nameText);
	}

	explorerTexture.display();
	explorerSprite.setTexture(explorerTexture.getTexture());
	target.draw(explorerSprite);
}

ViewExplorer::~ViewExplorer()
{
}

void ViewExplorer::loadViews()
{
	for (auto& i : std::experimental::filesystem::directory_iterator("."))		//dot means current directory. C++17 only, experimental approach, but I used old solutions and I'd rather avoid that
	{
		if (i.path().extension() == ".txt")
		{
			loadViewData(i.path().filename().generic_string());		//using these long namespaces and types is tiring and one day "experimental" namespace will be removed, so I converted it to standard string at the first opportunity. If implementation changes it will require changes only in this function (I hope)
		}
	}

	for (auto& i : std::experimental::filesystem::directory_iterator("."))		//unfortunately directory_iterator can access files in random order so I must search for connected images in the second loop
	{
		if (i.path().extension() == ".png")
		{
			loadViewImage(i.path().filename().generic_string());
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
	values.pop_back();		//the last one is always empty. Probably can be more elegant. TODO

	filename.erase(filename.end() - 4, filename.end());		//cut.png
	ViewRepresentation temp{ View{values}, filename };
	temp.nameText.setCharacterSize(20);
	temp.nameText.setString(filename);
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
	int previewWidth = columnWidth * 0.8f;
	int previewMargin = columnWidth * 0.1f;

	float newScale;

	if (represesentations.size() > 0)
	{
		newScale = previewWidth / float(represesentations[0].previewTexture.getSize().x);
	}

	for (size_t i = 0; i < represesentations.size(); i++)
	{
		int actualColumn = i%columns;
		int actualRow = i / columns;
		auto &sprite = represesentations[i].previewSprite;
		sprite.setTexture(represesentations[i].previewTexture);
		sprite.setScale(newScale, newScale);
		sprite.setPosition(margin + columnWidth*actualColumn + previewMargin, margin + columnWidth*actualRow + previewMargin);
		sf::FloatRect textArea{ sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + sprite.getGlobalBounds().height, sprite.getGlobalBounds().width, nameHeight };		//area which contains name of representation
		represesentations[i].nameText.setFont(arial);
		represesentations[i].nameText.setColor(sf::Color::Black);
		represesentations[i].nameText.setPosition(centerIn(represesentations[i].nameText.getLocalBounds(), textArea));
	}
}

sf::Vector2f ViewExplorer::centerIn(sf::FloatRect centeredObject, sf::FloatRect area) const
{
	if (area.width < centeredObject.width || area.height < centeredObject.height)
		throw std::range_error("Object must be smaller than area");
	else
		return sf::Vector2f(area.left + (area.width - centeredObject.width) / 2, area.top + (area.height - centeredObject.height) / 2);
}
