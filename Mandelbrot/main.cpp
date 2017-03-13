#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <fstream>
#include <thread>


sf::Image Result;
int resolution = 768;
bool isComputed = false;
double MandelbrotRadius = 2;
sf::Vector2<double> Center(0, 0);

void compute()
{
	std::cout << "X: " << Center.x << " Y: " << Center.y << " Radius: " << MandelbrotRadius << "\n";
	int iMax = 100;

	for (int stepY = 0; stepY < resolution; stepY++) 
	{
		for (int stepX = 0; stepX < resolution; stepX++)
		{
			double c_re = Center.x+((stepX - resolution / MandelbrotRadius)*MandelbrotRadius * 2 / resolution);
			double c_im = (-Center.y)+((stepY - resolution / MandelbrotRadius)*MandelbrotRadius * 2 / resolution);
			double x = 0, y = 0;
			int i = 0;

			while (hypot(x, y) <= 4 && i < iMax)
			{
				double x_new = x*x - y*y + c_re;
				y = 2 * x*y + c_im;
				x = x_new;
				i++;
			}

			if (i < iMax)
			{
				double iLin = 255 / (double)iMax*i;
				Result.setPixel(stepX, stepY, sf::Color(iLin, sin(iLin), cos(iLin)));
			}
			else
			{
				Result.setPixel(stepX, stepY, sf::Color::Black);
			}
		}
	}

	isComputed = true;
}

int main(int argc, char** argv)
{
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

	std::thread computing;

	sf::Clock clock;
	bool selectedCenter = false, clickedLeft=false;
	sf::Vector2<double> newCenter(Center.x, Center.y);
	sf::Vector2i pixelCenter;
	double newRadius = MandelbrotRadius;

	sf::VertexArray radiusFrame(sf::PrimitiveType::LinesStrip, 5);
	radiusFrame[0].color = sf::Color::Green;
	radiusFrame[1].color = sf::Color::Green;
	radiusFrame[2].color = sf::Color::Green;
	radiusFrame[3].color = sf::Color::Green;
	radiusFrame[4].color = sf::Color::Green;

	while (Window.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		sf::Event Event;
		sf::Mouse mouse;

		while (Window.pollEvent(Event))
		{
			if (Event.type == sf::Event::Closed || (Event.type == sf::Event::KeyReleased && Event.key.code == sf::Keyboard::Escape))
				Window.close();
			if (Event.type == sf::Event::KeyReleased && Event.key.code == sf::Keyboard::Return)
			{
				Center = newCenter;
				MandelbrotRadius = newRadius;
				isComputed = false;
				computing = std::thread(compute);
				computing.detach();
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
			double y = (GraphDimensions.top+GraphDimensions.height-mouse.getPosition().y) / (double)GraphDimensions.height*(MandelbrotRadius * 2) + Center.y - MandelbrotRadius;
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

		Window.clear(sf::Color::White);
		Window.draw(ResultSprite);
		Window.draw(centerText);
		Window.draw(radiusText);
		Window.draw(radiusFrame);
		Window.display();
	}

	return 0;
}