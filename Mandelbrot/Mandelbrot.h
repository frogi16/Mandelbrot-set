#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "TextField.h"
#include "StateButton.h"
#include "Button.h"

#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <thread>
#include <complex>
#include <math.h>

struct View
{
	int resolution{ 768 }, iterations{ 128 };
	double radius{ 2 };		//half of the side of the square
	sf::Vector2<double> center{ -0.5, 0 };
	char color{ 'R' };
};

class Mandelbrot
{
public:
	Mandelbrot();
	void init(int argc, char** argv);
	void init();
	void loop();
	~Mandelbrot();
private:
	void handleEvents(sf::Event &event);
	void update(sf::Mouse &mouse);
	void handleClicks();
	void draw();
	void compute(View &settings);
	void startThread();
	void clearFrame();
	void exportCoordinates();
	void exportImage();

	std::shared_ptr<Button> undoButton, resetButton, generateButton, exportButton;
	std::shared_ptr<StateButton> colorScheme;
	std::shared_ptr<TextField> iterationsField, resolutionField;
	std::shared_ptr<sf::Text> iterationsTitle, resolutionTitle, colorsTitle, loading;

	std::vector<std::shared_ptr<Button>> buttons;
	std::vector<std::shared_ptr<StateButton>> stateButtons;
	std::vector<std::shared_ptr<TextField>> textFields;
	std::vector<std::shared_ptr<sf::Text>> texts;

	View defaultView,		//reset 
		previousView,		//undo
		currentView,		//export
		nextView;			//temporary storage

	bool isComputing = false, isComputed = false, selectedCenter = false, clickedLeft = false, closeApplication = false;

	sf::VideoMode desktop;
	sf::RenderWindow window;

	sf::Clock clock;

	sf::IntRect graphDimensions;
	sf::IntRect settingsDimensions;

	sf::Image result;
	sf::Texture resultTexture;
	sf::Sprite resultSprite;

	sf::Font arial;
	sf::Text centerText;
	sf::Text radiusText;

	sf::Vector2i pixelCenter;

	sf::VertexArray radiusFrame;

	std::thread computing;
};

