#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "TextField.h"
#include "StateButton.h"

#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <thread>
#include <complex>
#include <math.h>

sf::Image Result;
int resolution = 768;
bool isComputed = false;
double MandelbrotRadius = 2;
sf::Vector2<double> Center(-0.5, 0);
TextField iterationsField(sf::IntRect(20, 100, 100, 40), FieldType::Number, 1);
StateButton colorScheme(sf::IntRect(140, 100, 40, 40));

void compute()
{
	int iMax = iterationsField.getValueInt();
	double unit = 2 * MandelbrotRadius / (double)resolution;

	for (int stepY = 0; stepY < resolution; stepY++)
	{
		for (int stepX = 0; stepX < resolution; stepX++)
		{
			double x = Center.x - MandelbrotRadius + stepX*unit;
			double y = Center.y - MandelbrotRadius + stepY*unit;

			double c_re = x;
			double c_im = y;
			x = 0;
			y = 0;
			int i = 0;
			while (x*x + y*y <= 4 && i < iMax)
			{
				double x_new = x*x - y*y + c_re;
				y = 2 * x*y + c_im;
				x = x_new;
				i++;
			}

			if (i < iMax)
			{
				double iLin = 255 / (double)iMax*i;
				
				if (colorScheme.getState() == 'R')
				{
					Result.setPixel(stepX, resolution - stepY - 1, sf::Color(iLin, sin(iLin), cos(iLin)));
				}
				else if(colorScheme.getState() == 'G')
				{
					Result.setPixel(stepX, resolution - stepY - 1, sf::Color(sin(iLin), iLin, cos(iLin)));
				}
				else if (colorScheme.getState() == 'B')
				{
					Result.setPixel(stepX, resolution - stepY - 1, sf::Color(sin(iLin), cos(iLin), iLin));
				}
			}
			else
			{
				Result.setPixel(stepX, resolution - stepY - 1, sf::Color::Black);
			}
		}
	}
	isComputed = true;
}

int main(int argc, char** argv)
{
	if (argc == 4)
	{
		Center.x = std::stoi(argv[1]);
		Center.y = std::stoi(argv[2]);
		MandelbrotRadius = std::stoi(argv[3]);
	}

	sf::VideoMode Pulpit = sf::VideoMode::getDesktopMode();
	sf::RenderWindow Window(sf::VideoMode(Pulpit.width, Pulpit.height), "Mandelbrot", sf::Style::Fullscreen);

	sf::FloatRect GraphDimensions(Pulpit.width - Pulpit.height, 0, Pulpit.height, Pulpit.height);
	sf::FloatRect SettingsDimensions(0, 0, Pulpit.width - Pulpit.height - 2, Pulpit.height);

	Result.create(resolution, resolution, sf::Color::White);
	sf::Texture ResultTexture;
	sf::Sprite ResultSprite;
	ResultSprite.setPosition(GraphDimensions.left, GraphDimensions.top);

	sf::Font arial;
	arial.loadFromFile("arial.ttf");

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

	iterationsField.setString("100");

	std::thread computing;
	isComputed = false;
	computing = std::thread(compute);
	computing.detach();

	sf::Clock clock;
	bool selectedCenter = false, clickedLeft = false;
	sf::Vector2<double> newCenter(Center.x, Center.y);
	sf::Vector2i pixelCenter;
	double newRadius = MandelbrotRadius;

	sf::VertexArray radiusFrame(sf::PrimitiveType::LinesStrip, 5);

	for (size_t i = 0; i < 5; i++)
	{
		radiusFrame[i].color = sf::Color::Green;
	}

	while (Window.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		sf::Event Event;
		sf::Mouse mouse;

		while (Window.pollEvent(Event))
		{
			if (Event.type == sf::Event::Closed || (Event.type == sf::Event::KeyReleased && Event.key.code == sf::Keyboard::Escape))
				Window.close();
			else if (Event.type == sf::Event::KeyReleased && Event.key.code == sf::Keyboard::Return)
			{
				Center = newCenter;
				MandelbrotRadius = newRadius;
				isComputed = false;
				computing = std::thread(compute);
				computing.detach();

				for (size_t i = 0; i < 5; i++)
				{
					radiusFrame[i].position = sf::Vector2f(0, 0);
				}
			}
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
			ResultTexture.loadFromImage(Result);
			ResultSprite.setTexture(ResultTexture, true);
			isComputed = false;
		}

		if (GraphDimensions.contains(mouse.getPosition().x, mouse.getPosition().y))
		{
			double x = (mouse.getPosition().x - GraphDimensions.left) / (double)GraphDimensions.width*(MandelbrotRadius * 2) + Center.x - MandelbrotRadius;
			double y = (GraphDimensions.top + GraphDimensions.height - mouse.getPosition().y) / (double)GraphDimensions.height*(MandelbrotRadius * 2) + Center.y - MandelbrotRadius;
			centerText.setString("X: " + std::to_string(x) + " Y: " + std::to_string(y));

			if (!selectedCenter && !mouse.isButtonPressed(sf::Mouse::Button::Left) && clickedLeft)
			{
				pixelCenter.x = mouse.getPosition().x;
				pixelCenter.y = mouse.getPosition().y;
				clickedLeft = false;
				selectedCenter = true;
				newCenter.x = x;
				newCenter.y = y;
			}

			if (selectedCenter)
			{
				int distance = hypot(mouse.getPosition().x - pixelCenter.x, mouse.getPosition().y - pixelCenter.y);
				radiusFrame[0].position = sf::Vector2f(pixelCenter.x - distance, pixelCenter.y - distance);
				radiusFrame[1].position = sf::Vector2f(pixelCenter.x + distance, pixelCenter.y - distance);
				radiusFrame[2].position = sf::Vector2f(pixelCenter.x + distance, pixelCenter.y + distance);
				radiusFrame[3].position = sf::Vector2f(pixelCenter.x - distance, pixelCenter.y + distance);
				radiusFrame[4].position = sf::Vector2f(pixelCenter.x - distance, pixelCenter.y - distance);

				radiusText.setString("Radius: " + std::to_string(distance / GraphDimensions.width*MandelbrotRadius * 2));

				if (!mouse.isButtonPressed(sf::Mouse::Button::Left) && clickedLeft)
				{
					clickedLeft = false;
					selectedCenter = false;
					newRadius = distance / GraphDimensions.width*MandelbrotRadius * 2;
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

		Window.clear(sf::Color::White);
		Window.draw(ResultSprite);
		Window.draw(centerText);
		Window.draw(radiusText);
		Window.draw(radiusFrame);
		iterationsField.draw(Window);
		colorScheme.draw(Window);
		Window.display();
	}

	return 0;
}