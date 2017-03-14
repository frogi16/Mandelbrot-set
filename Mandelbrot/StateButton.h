#pragma once
#include "GUIElement.h"

#include <SFML\Window\Mouse.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\Text.hpp>

class StateButton :
	public GUIElement
{
public:
	StateButton(sf::IntRect dimensions);
	~StateButton();
	Informations getInformations() { return Informations{ true, false }; }
	void updateMouse(const sf::Mouse& mouse);
	void draw(sf::RenderTarget& target);
	char getState() { return states[buttonState]; }
private:
	const char states[3]{ 'R', 'G', 'B' };
	void changeState();
	bool isGrabbed;
	int buttonState;
	sf::RectangleShape button;
	sf::Font arial;
	sf::Text buttonText;
};

