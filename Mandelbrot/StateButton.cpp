#include "StateButton.h"



StateButton::StateButton(sf::IntRect dimensions) :
	button(sf::Vector2f(dimensions.width, dimensions.height)),
	buttonState(-1),		//after change it will be 0
	isGrabbed(false)
{
	arial.loadFromFile("arial.ttf");

	buttonText.setFont(arial);
	buttonText.setColor(sf::Color::Black);
	buttonText.setCharacterSize(dimensions.height / 3);

	button.setPosition(dimensions.left, dimensions.top);
	button.setFillColor(sf::Color::White);
	button.setOutlineColor(sf::Color::Black);
	button.setOutlineThickness(1);

	changeState();
}

StateButton::~StateButton()
{
}

void StateButton::updateMouse(const sf::Mouse & mouse)
{
	bool isHovered = (button.getGlobalBounds().contains(sf::Vector2f(mouse.getPosition().x, mouse.getPosition().y)));

	if (isGrabbed&&!mouse.isButtonPressed(sf::Mouse::Button::Left))
	{
		changeState();
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

void StateButton::draw(sf::RenderTarget & target)
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

void StateButton::changeState()
{
	if (buttonState == 2)
	{
		buttonState = 0;
	}
	else
	{
		buttonState++;
	}

	buttonText.setString(states[buttonState]);
	buttonText.setOrigin(buttonText.getLocalBounds().left + buttonText.getLocalBounds().width / 2, buttonText.getLocalBounds().top + buttonText.getLocalBounds().height / 2);	//center the origin point
	buttonText.setPosition(button.getPosition().x + button.getSize().x / 2, button.getPosition().y + button.getSize().y / 2);	//set the text's origin's position exactly as box's origin's position
}
