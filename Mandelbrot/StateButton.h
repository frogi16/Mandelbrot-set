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
	void changeState(const char target);
private:
	const char states[3]{ 'R', 'G', 'B' };
	void changeState();
	void updateText();
	bool isGrabbed;
	int buttonState;
	sf::RectangleShape button;
	sf::Font arial;
	sf::Text buttonText;
};

