#pragma once

#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "TextField.h"
#include "StateButton.h"
#include "Button.h"

#include "View.h"

class Mandelbrot;

class ViewExplorer
{
public:
	ViewExplorer(sf::IntRect dimen, Mandelbrot& own);
	ViewExplorer(const ViewExplorer&) = delete; // non construction-copyable
	ViewExplorer& operator=(const ViewExplorer&) = delete; // non copyable
	void handleMouse(const sf::Mouse &mouse);
	void scroll(const sf::Event &event);
	void draw(sf::RenderTarget &target);
	void deselect() { selectedRepresentation = nullptr; }
	const sf::IntRect& getDimensions() { return dimensions; }
	~ViewExplorer();
private:
	void loadViews();
	void loadViewData(std::string filename);
	void loadViewImage(std::string filename);
	void orderSprites();
	void setNameTextProperties(sf::Text &nameText, const sf::FloatRect &area, const std::string &name);
	void setSelectedFrameProperties(int width, int height);
	bool previewExists(const std::string &name);
	sf::Vector2f centerIn(const sf::FloatRect &centeredObject, const sf::FloatRect &area) const;
	sf::Vector2f toLocalCoordinates(const sf::Vector2f &glob) { return sf::Vector2f{ glob.x - dimensions.left, glob.y - dimensions.top }; }
	sf::Vector2i toLocalCoordinates(const sf::Vector2i &glob) { return sf::Vector2i{ glob.x - dimensions.left, glob.y - dimensions.top }; }

	Mandelbrot &owner;
	std::shared_ptr<Button> reloadButton;
	std::vector<std::shared_ptr<Button>> buttons;

	const int columns;
	int realHeight, scrollPosition = 0;
	bool scrollable = false;
	const sf::IntRect dimensions;
	std::vector<ViewRepresentation> represesentations;
	ViewRepresentation *grabbedRepresentation = nullptr;		//way to handle hovering and clicking by this kind of variables is borrowed from Button class. Probably stupid and ignorant,  but works
	ViewRepresentation *selectedRepresentation = nullptr;
	sf::RectangleShape selectedFrame;
	sf::RenderTexture explorerTexture;
	sf::Sprite explorerSprite;
	sf::Font arial;
};

