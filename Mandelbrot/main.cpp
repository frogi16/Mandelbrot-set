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

sf::Image result;
bool isComputed = false;

struct View
{
	int resolution{ 768 }, iterations{ 128 };
	double radius{ 2 };		//half of the side of the square
	sf::Vector2<double> center{ -0.5, 0 };
	char color{ 'R' };
};

void compute(View &settings)
{
	int iMax = settings.iterations;
	double unit = 2 * settings.radius / (double)settings.resolution;

	for (int stepY = 0; stepY < settings.resolution; stepY++)
	{
		for (int stepX = 0; stepX < settings.resolution; stepX++)
		{
			double x = settings.center.x - settings.radius + stepX*unit;
			double y = settings.center.y - settings.radius + stepY*unit;

			double c_re = x;
			double c_im = y;
			x = 0;
			y = 0;
			int i = 0;
			while (x*x + y*y <= 4 && i < iMax)
			{
				double x_new = pow(x,2) - pow(y,2) + c_re;
				y = 2 * x*y + c_im;
				x = x_new;
				i++;
			}

			if (i < iMax)
			{
				double iLin = 255 / (double)iMax*i;
				
				switch (settings.color)
				{
				case 'R':
					result.setPixel(stepX, settings.resolution - stepY - 1, sf::Color(iLin, sin(iLin), cos(iLin)));
					break;
				case 'G':
					result.setPixel(stepX, settings.resolution - stepY - 1, sf::Color(sin(iLin), iLin, cos(iLin)));
					break;
				case 'B':
					result.setPixel(stepX, settings.resolution - stepY - 1, sf::Color(sin(iLin), cos(iLin), iLin));
					break;
				default:
					break;
				}
			}
			else
			{
				result.setPixel(stepX, settings.resolution - stepY - 1, sf::Color::Black);
			}
		}
	}
	isComputed = true;
}

int main(int argc, char** argv)
{
	View previousView,		//undo
		currentView,		//export
		nextView;			//temporary storage

	if (argc == 4)
	{
		currentView.center.x = std::stoi(argv[1]);
		currentView.center.y = std::stoi(argv[2]);
		currentView.radius = std::stoi(argv[3]);
	}

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::RenderWindow window(sf::VideoMode(desktop.width, desktop.height), "Mandelbrot", sf::Style::Fullscreen);

	sf::FloatRect graphDimensions(desktop.width - desktop.height, 0, desktop.height, desktop.height);
	sf::FloatRect settingsDimensions(0, 0, desktop.width - desktop.height - 2, desktop.height);
	currentView.resolution = graphDimensions.width;

	result.create(currentView.resolution, currentView.resolution, sf::Color::White);
	sf::Texture resultTexture;
	sf::Sprite resultSprite;
	resultSprite.setPosition(graphDimensions.left, graphDimensions.top);

	sf::Font arial;
	arial.loadFromFile("arial.ttf");

	Button undoButton(sf::IntRect(200, 100, 120, 40), "Undo");
	Button resetButton(sf::IntRect(340, 100, 120, 40), "Reset");
	Button generateButton(sf::IntRect(480, 100, 120, 40), "Generate");

	sf::Text centerText;
	centerText.setFont(arial);
	centerText.setColor(sf::Color::Black);
	centerText.setPosition(0, 0);
	centerText.setCharacterSize(24);

	sf::Text radiusText;
	radiusText.setFont(arial);
	radiusText.setColor(sf::Color::Black);
	radiusText.setPosition(0, 30);
	radiusText.setCharacterSize(24);

	TextField iterationsField(sf::IntRect(20, 100, 100, 40), FieldType::Number, 1);
	iterationsField.setString("128");

	StateButton colorScheme(sf::IntRect(140, 100, 40, 40));

	std::thread computing;
	isComputed = false;
	computing = std::thread(compute, currentView);
	computing.detach();

	sf::Clock clock;
	bool selectedCenter = false, clickedLeft = false;

	const View defaultView = currentView;		//needed to reset settings
	nextView = currentView;

	sf::Vector2i pixelCenter;

	sf::VertexArray radiusFrame(sf::PrimitiveType::LinesStrip, 5);

	for (size_t i = 0; i < 5; i++)
	{
		radiusFrame[i].color = sf::Color::Green;
	}

	while (window.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		sf::Event Event;
		sf::Mouse mouse;

		while (window.pollEvent(Event))
		{
			if (Event.type == sf::Event::Closed || (Event.type == sf::Event::KeyReleased && Event.key.code == sf::Keyboard::Escape))		//exit
				window.close();
			else if (Event.type == sf::Event::TextEntered && Event.text.unicode != 8 && Event.text.unicode != 10 && Event.text.unicode != 13)		//backspace, space, enter
			{
				iterationsField.updateKeyboard(Event);
			}
			else if (Event.type == sf::Event::KeyPressed && Event.key.code == sf::Keyboard::BackSpace)
			{
				iterationsField.eraseCharacter();
			}
		}

		if (isComputed)
		{
			resultTexture.loadFromImage(result);
			resultSprite.setTexture(resultTexture, true);
			isComputed = false;
		}

		if (graphDimensions.contains(mouse.getPosition().x, mouse.getPosition().y))
		{
			double x = (mouse.getPosition().x - graphDimensions.left) / (double)graphDimensions.width*(currentView.radius * 2) + currentView.center.x - currentView.radius;
			double y = (graphDimensions.top + graphDimensions.height - mouse.getPosition().y) / (double)graphDimensions.height*(currentView.radius * 2) + currentView.center.y - currentView.radius;
			centerText.setString("X: " + std::to_string(x) + " Y: " + std::to_string(y));

			if (!selectedCenter && !mouse.isButtonPressed(sf::Mouse::Button::Left) && clickedLeft)
			{
				pixelCenter.x = mouse.getPosition().x;
				pixelCenter.y = mouse.getPosition().y;
				clickedLeft = false;
				selectedCenter = true;
				nextView.center.x = x;
				nextView.center.y = y;
			}

			if (selectedCenter)
			{
				int distance = hypot(mouse.getPosition().x - pixelCenter.x, mouse.getPosition().y - pixelCenter.y);
				radiusFrame[0].position = sf::Vector2f(pixelCenter.x - distance, pixelCenter.y - distance);
				radiusFrame[1].position = sf::Vector2f(pixelCenter.x + distance, pixelCenter.y - distance);
				radiusFrame[2].position = sf::Vector2f(pixelCenter.x + distance, pixelCenter.y + distance);
				radiusFrame[3].position = sf::Vector2f(pixelCenter.x - distance, pixelCenter.y + distance);
				radiusFrame[4].position = sf::Vector2f(pixelCenter.x - distance, pixelCenter.y - distance);

				radiusText.setString("Radius: " + std::to_string(distance / graphDimensions.width*currentView.radius * 2));

				if (!mouse.isButtonPressed(sf::Mouse::Button::Left) && clickedLeft)
				{
					clickedLeft = false;
					selectedCenter = false;
					nextView.radius = distance / graphDimensions.width*currentView.radius * 2;
				}
				if (mouse.isButtonPressed(sf::Mouse::Button::Left))
					clickedLeft = true;
			}

			if (!selectedCenter && mouse.isButtonPressed(sf::Mouse::Button::Left))
			{
				clickedLeft = true;
			}
		}

		iterationsField.updateMouse(mouse);
		colorScheme.updateMouse(mouse);

		undoButton.updateMouse(mouse);
		if (undoButton.clicked())
		{
			iterationsField.setString(std::to_string(previousView.iterations));
			nextView = previousView;
		}

		resetButton.updateMouse(mouse);
		if (resetButton.clicked())
		{
			iterationsField.setString(std::to_string(defaultView.iterations));
			nextView = defaultView;
		}

		generateButton.updateMouse(mouse);
		if (generateButton.clicked())
		{
			nextView.color = colorScheme.getState();
			nextView.iterations = iterationsField.getValueInt();

			previousView = std::move(currentView);
			currentView = std::move(nextView);

			isComputed = false;
			computing = std::thread(compute, currentView);
			computing.detach();

			for (size_t i = 0; i < 5; i++)
			{
				radiusFrame[i].position = sf::Vector2f(0, 0);
			}
		}

		window.clear(sf::Color::White);
		window.draw(resultSprite);
		window.draw(centerText);
		window.draw(radiusText);
		window.draw(radiusFrame);
		iterationsField.draw(window);
		colorScheme.draw(window);
		undoButton.draw(window);
		resetButton.draw(window);
		generateButton.draw(window);
		window.display();
	}

	return 0;
}