#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <fstream>
#include <thread>

sf::Vector2<double> corner1(-2.5, -1), corner2(1, 1), mandelbrotCorner(-1, -0.5);
sf::Image Result;
int resolution = 768;
bool done = false;

void compute()
{
	int zeroX = abs(corner1.x) / (abs(corner1.x) + corner2.x)*resolution;
	int zeroY = abs(corner1.y) / (abs(corner1.y) + corner2.y)*resolution;

	for (size_t StepY = 1; StepY <= resolution; StepY++)
	{
		for (size_t StepX = 1; StepX <= resolution; StepX++)
		{
			double x = corner1.x + (corner2.x / resolution * StepX);
			double y = corner1.y + (corner2.y / resolution * StepY);
			double sX = x - mandelbrotCorner.x;
			double sY = y - mandelbrotCorner.y;		//coordinates scaled to lie in Mandelbrot scale
			
			if (StepX == resolution/2)
			{
				bool test = 1;
			}

			if (StepX == zeroX && StepY == zeroY)
			{
				bool test2 = true;
			}

			x = 0;
			y = 0;

			int i = 0, maxI = 100;

			while (hypot(x, y) < 4 && i < maxI)
			{
				double xTemp = x*x - y*y + sX;
				y = 2 * x*y + sY;
				x = xTemp;
				i++;
			}

			Result.setPixel(StepX-1, StepY-1, sf::Color(255 / maxI*i, 1/(255 / maxI*i), 0));

			if (StepX == zeroX || StepY == zeroY)
			{
				Result.setPixel(StepX - 1, StepY - 1, sf::Color::White);
			}
		}
	}
	done = true;
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

	std::thread computing;
	computing = std::thread(compute);
	computing.detach();

	sf::Clock clock;

	while (Window.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		sf::Event Event;

		while (Window.pollEvent(Event))
		{
			if (Event.type == sf::Event::Closed || (Event.type == sf::Event::KeyReleased && Event.key.code == sf::Keyboard::Escape))
				Window.close();
		}

		if (done==true)
		{
			ResultTexture.loadFromImage(Result);
			ResultSprite.setTexture(ResultTexture, true);
		}

		Window.clear(sf::Color::White);
		Window.draw(ResultSprite);
		Window.display();
	}

	return 0;
}