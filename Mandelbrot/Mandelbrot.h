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

struct View		//this structure is a legacy after ther original way of generating the Mandelbrot set and I decided to leave it as it is. Change it only after extensive rebuilding of Mandelbrot::compute(...) and selection system.
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
	Mandelbrot(const Mandelbrot&) = delete; // non construction-copyable
	Mandelbrot& operator=(const Mandelbrot&) = delete; // non copyable
	void init(const int argc, char** const argv);
	void loop();
	~Mandelbrot();
private:
	void init();
	void handleEvents(sf::Event &event);
	void update(sf::Mouse &mouse);
	void handleClicks();
	void draw();
	void compute(const View &settings);
	void adjustResolution();
	void startThread();
	void clearFrame();
	void exportCoordinates(std::string filename) const;
	void exportImage(std::string filename);

	std::shared_ptr<Button> undoButton, resetButton, generateButton, exportButton;
	std::shared_ptr<StateButton> colorScheme;
	std::shared_ptr<TextField> iterationsField, resolutionField, filenameField;
	std::shared_ptr<sf::Text> iterationsTitle, resolutionTitle, colorsTitle, filenameTitle, loading;

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

	std::thread computing, exporting;
};

