#pragma once

#include "ViewExplorer.h"

#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <thread>
#include <complex>
#include <math.h>

class Mandelbrot
{
public:
	Mandelbrot(int argc, char ** argv);
	Mandelbrot(const Mandelbrot&) = delete; // non construction-copyable
	Mandelbrot& operator=(const Mandelbrot&) = delete; // non copyable
	void loop();
	void changeCurrentView(const View &data, const sf::Texture & previewTexture);
	~Mandelbrot();
private:
	void handleEvents(sf::Event &event);
	void update(sf::Mouse &mouse);
	void handleClicks();
	void draw();
	void prepareImage();
	void compute(const View &settings);
	void adjustResolution();
	void scaleResultSprite();
	void startThread();
	void clearFrame();
	void exportView(std::string filename) const;
	void exportImage(std::string filename);

	std::shared_ptr<Button> undoButton, resetButton, generateButton, exportButton;
	std::shared_ptr<StateButton> colorScheme;
	std::shared_ptr<TextField> iterationsField, resolutionField, filenameField;
	std::shared_ptr<sf::Text> iterationsTitle, resolutionTitle, colorsTitle, filenameTitle, loading;

	std::vector<std::shared_ptr<Button>> buttons;
	std::vector<std::shared_ptr<StateButton>> stateButtons;
	std::vector<std::shared_ptr<TextField>> textFields;
	std::vector<std::shared_ptr<sf::Text>> texts;

	std::unique_ptr<ViewExplorer> viewExplorer;

	View defaultView,		//reset 
		previousView,		//undo
		currentView,		//export
		nextView;			//temporary storage

	bool isComputing = false, computed = false, selectedCenter = false, clickedLeft = false, closeApplication = false;

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

