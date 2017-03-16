#pragma once
#include "GUIElement.h"

#include <SFML\Window\Mouse.hpp>
#include <SFML\Graphics\RectangleShape.hpp>
#include <SFML\Graphics\Text.hpp>

class Button :
	public GUIElement
{
public:
	Button(sf::IntRect dimensions, std::string text);
	~Button();
	Informations getInformations() { return Informations{ true, false }; }
	bool clicked() { return isClicked; }
	void updateMouse(const sf::Mouse& mouse);
	void draw(sf::RenderTarget& target);
private:
	bool isGrabbed, isClicked;
	sf::RectangleShape button;
	sf::Font arial;
	sf::Text buttonText;
};

