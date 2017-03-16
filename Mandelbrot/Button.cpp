#include "Button.h"


Button::Button(sf::IntRect dimensions, std::string text) :
	button(sf::Vector2f(dimensions.width, dimensions.height)),
	isGrabbed(false),
	isClicked(false)
{
	arial.loadFromFile("arial.ttf");

	buttonText.setFont(arial);
	buttonText.setString(text);
	buttonText.setColor(sf::Color::Black);
	buttonText.setCharacterSize(dimensions.height/3);

	button.setPosition(dimensions.left, dimensions.top);
	button.setFillColor(sf::Color::White);
	button.setOutlineColor(sf::Color::Black);
	button.setOutlineThickness(1);

	buttonText.setOrigin(buttonText.getLocalBounds().left + buttonText.getLocalBounds().width / 2, buttonText.getLocalBounds().top + buttonText.getLocalBounds().height / 2);	//center the origin point
	buttonText.setPosition(button.getPosition().x + button.getSize().x / 2, button.getPosition().y + button.getSize().y / 2);	//set the text's origin's position exactly as box's origin's position
}

Button::~Button()
{
}

void Button::updateMouse(const sf::Mouse & mouse)
{
	bool isHovered = (button.getGlobalBounds().contains(sf::Vector2f(mouse.getPosition().x, mouse.getPosition().y)));

	if (isGrabbed&&!mouse.isButtonPressed(sf::Mouse::Button::Left))
	{
		isClicked = true;
	}
	else
	{
		isClicked = false;
	}

	if (isHovered&&mouse.isButtonPressed(sf::Mouse::Button::Left))
	{
		isGrabbed = true;
	}
	else
	{
		isGrabbed = false;
	}
}

void Button::draw(sf::RenderTarget & target)
{
	if (isGrabbed)
	{
		button.setFillColor(sf::Color(225, 225, 225));
	}
	else
	{
		button.setFillColor(sf::Color::White);
	}
	target.draw(button);
	target.draw(buttonText);
}
