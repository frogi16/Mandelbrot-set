#include "TextField.h"



TextField::TextField(sf::IntRect dimensions, FieldType type, int textLines) :
	dimensions(dimensions),
	type(type),
	textLines(textLines),
	isActive(false),
	isHovered(true),
	errorTime(sf::seconds(3))
{
	arial.loadFromFile("arial.ttf");

	box.setPosition(dimensions.left, dimensions.top);
	box.setSize(sf::Vector2f(dimensions.width, dimensions.height));
	box.setOutlineColor(sf::Color::Black);
	box.setOutlineThickness(1);

	contentText.setColor(sf::Color::Black);
	contentText.setCharacterSize(dimensions.height*0.9f / textLines);
	contentText.setPosition(box.getPosition());
	contentText.setFont(arial);
	contentText.setString("0");
	actualizeText();
}

void TextField::updateMouse(const sf::Mouse & mouse)
{
	isHovered = (dimensions.contains(mouse.getPosition()));

	if (mouse.isButtonPressed(sf::Mouse::Left))
	{
		isActive = isHovered;
	}
}

void TextField::updateKeyboard(sf::Event & event)
{
	if (isActive && isTypeMatching(event))
	{
		contentText.setString(contentText.getString() + event.text.unicode);
		actualizeText();
	}
}

void TextField::eraseCharacter()
{
	if (isActive)
	{
		std::string temp = contentText.getString();

		if (temp.size() > 0)
		{
			contentText.setString(temp.erase(temp.size() - 1, 1));
			actualizeText();
		}
	}
}

void TextField::draw(sf::RenderTarget & target)
{
	if (errorTimer.getElapsedTime().asSeconds()>errorTime.asSeconds())
	{
		box.setOutlineColor(sf::Color::Black);
	}

	target.draw(box);
	target.draw(contentText);
}

void TextField::showError()
{
	box.setOutlineColor(sf::Color::Red);
	errorTimer.restart();
}

std::string TextField::getValueString()
{
	std::string temp = contentText.getString();
	temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
	return temp;
}

bool TextField::isTypeMatching(sf::Event & event)
{
	if (type == FieldType::Number)
	{
		if ((event.text.unicode >= '0' && event.text.unicode <= '9') || event.text.unicode == 45)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

void TextField::actualizeText()
{
	std::string cutted;
	bool isCutted = false;

	while(contentText.getLocalBounds().width > dimensions.width)
	{
		cutted.push_back(contentText.getString().toAnsiString().back());
		std::string newString = contentText.getString();
		newString.pop_back();
		contentText.setString(newString);
		isCutted = true;
	}

	if (isCutted)
	{
		std::reverse(cutted.begin(), cutted.end());
		contentText.setString(contentText.getString() + "\n" + cutted);
	}

	contentText.setOrigin(contentText.getLocalBounds().left + contentText.getLocalBounds().width / 2, contentText.getLocalBounds().top + contentText.getLocalBounds().height / 2);	//center the origin point
	contentText.setPosition(box.getPosition().x + box.getSize().x / 2, box.getPosition().y + box.getSize().y / 2);	//set the text's origin's position exactly as box's origin's position

	if(contentText.getLocalBounds().width > dimensions.width)
		actualizeText();
}