#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "TextField.h"
#include "StateButton.h"
#include "Button.h"

struct View		//this structure is a legacy after ther original way of generating the Mandelbrot set and I decided to leave it as it is. Change it only after extensive rebuilding of Mandelbrot::compute(...) and selection system.
{
	View() {}
	View(std::vector<std::string> &values) : radius(std::stod(values[0])), center(std::stod(values[1]), std::stod(values[2]))
	{
		if (values.size() == 6)
		{
			resolution = std::stoi(values[3]);
			iterations = std::stoi(values[4]);
			if (values[5].size() == 1)
			{
				color = values[5][0];
			}
		}
	}

	int resolution{ 768 }, iterations{ 128 };
	double radius{ 2 };		//half of the side of the square
	sf::Vector2<double> center{ -0.5, 0 };
	char color{ 'R' };

	std::string getBasicString() const { return std::string{ std::to_string(radius) + " " + std::to_string(center.x) + " " + std::to_string(center.y) + "\n" }; }	//info required to computations
	std::string getFullString() const { return getBasicString() + std::to_string(resolution) + " " + std::to_string(iterations) + " " + color; }	//info about the presentation of result
};

struct ViewRepresentation
{
	//ViewRepresentation()
	View data;
	std::string name = {};
	sf::Texture previewTexture = {};
	sf::Sprite previewSprite = {};
	sf::Text nameText = {};
};

class ViewExplorer
{
public:
	ViewExplorer(sf::IntRect dimen);
	ViewExplorer(const ViewExplorer&) = delete; // non construction-copyable
	ViewExplorer& operator=(const ViewExplorer&) = delete; // non copyable
	void handleMouse(sf::Mouse mouse);
	void draw(sf::RenderTarget &target);
	~ViewExplorer();
private:
	int columns;
	void loadViews();
	void loadViewData(std::string filename);
	void loadViewImage(std::string filename);
	void orderSprites();
	sf::Vector2f centerIn(sf::FloatRect centeredObject, sf::FloatRect area) const;
	sf::Vector2f toLocalCoordinates(sf::Vector2f glob) { return sf::Vector2f{ glob.x - dimensions.width, glob.y - dimensions.height }; }
	sf::IntRect dimensions;
	std::vector<ViewRepresentation> represesentations;
	sf::RenderTexture explorerTexture;
	sf::Sprite explorerSprite;
	sf::Font arial;
};

