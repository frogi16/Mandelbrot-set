#include "Mandelbrot.h"



Mandelbrot::Mandelbrot()
{
}

void Mandelbrot::init(int argc, char ** argv)
{
	/*currentView.center.x = std::stoi(argv[1]);
	currentView.center.y = std::stoi(argv[2]);
	currentView.radius = std::stoi(argv[3]);*/
}

void Mandelbrot::init()
{
	desktop = sf::VideoMode::getDesktopMode();
	window.create(sf::VideoMode(desktop.width, desktop.height), "Mandelbrot", sf::Style::Fullscreen);

	graphDimensions = sf::IntRect(desktop.width - desktop.height, 0, desktop.height, desktop.height);
	settingsDimensions = sf::IntRect(0, 0, desktop.width - desktop.height - 2, desktop.height);
	currentView.resolution = graphDimensions.width;

	result.create(currentView.resolution, currentView.resolution, sf::Color::White);
	resultSprite.setPosition(graphDimensions.left, graphDimensions.top);

	arial.loadFromFile("arial.ttf");

	undoButton = std::make_shared<Button>(sf::IntRect(200, 100, 120, 40), "Undo");
	resetButton = std::make_shared<Button>(sf::IntRect(340, 100, 120, 40), "Reset");
	generateButton = std::make_shared<Button>(sf::IntRect(480, 100, 120, 40), "Generate");
	buttons.push_back(undoButton);
	buttons.push_back(resetButton);
	buttons.push_back(generateButton);

	colorScheme = std::make_shared<StateButton>(sf::IntRect(140, 100, 40, 40));
	iterationsField = std::make_shared<TextField>(sf::IntRect(20, 100, 100, 40), FieldType::Number, "128", 1);
	stateButtons.push_back(colorScheme);
	textFields.push_back(iterationsField);

	centerText.setFont(arial);
	centerText.setColor(sf::Color::Black);
	centerText.setPosition(0, 0);
	centerText.setCharacterSize(24);

	radiusText.setFont(arial);
	radiusText.setColor(sf::Color::Black);
	radiusText.setPosition(0, 30);
	radiusText.setCharacterSize(24);

	computing = std::thread(&Mandelbrot::compute, this, currentView);
	computing.detach();

	defaultView = currentView;		//needed to reset settings
	nextView = currentView;

	radiusFrame.setPrimitiveType(sf::PrimitiveType::LinesStrip);
	radiusFrame.resize(5);

	for (size_t i = 0; i < 5; i++)
	{
		radiusFrame[i].color = sf::Color::Green;
	}

}

void Mandelbrot::loop()
{
	while (window.isOpen() && !closeApplication)
	{
		sf::Time elapsedTime = clock.restart();
		sf::Event event;
		sf::Mouse mouse;

		handleEvents(event);

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

				radiusText.setString("Radius: " + std::to_string(distance / float(graphDimensions.width)*currentView.radius * 2));

				if (!mouse.isButtonPressed(sf::Mouse::Button::Left) && clickedLeft)
				{
					clickedLeft = false;
					selectedCenter = false;
					nextView.radius = distance / float(graphDimensions.width)*currentView.radius * 2;
				}
				if (mouse.isButtonPressed(sf::Mouse::Button::Left))
					clickedLeft = true;
			}

			if (!selectedCenter && mouse.isButtonPressed(sf::Mouse::Button::Left))
			{
				clickedLeft = true;
			}
		}

		update(mouse);
		handleClicks();
		draw();
	}
}

void Mandelbrot::handleEvents(sf::Event &event)
{
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape))		//exit
		{
			window.close();
			closeApplication = true;
		}
		else if (event.type == sf::Event::TextEntered && event.text.unicode != 8 && event.text.unicode != 10 && event.text.unicode != 13)		//backspace, space, enter
		{
			for (auto& i : textFields)
			{
				i->updateKeyboard(event);
			}
		}
		else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace)
		{
			for (auto& i : textFields)
			{
				i->eraseCharacter();
			}
		}
	}
}

void Mandelbrot::update(sf::Mouse &mouse)
{
	for (auto& i : buttons)
	{
		i->updateMouse(mouse);
	}

	for (auto& i : stateButtons)
	{
		i->updateMouse(mouse);
	}

	for (auto& i : textFields)
	{
		i->updateMouse(mouse);
	}
}

void Mandelbrot::handleClicks()
{
	if (undoButton->clicked())
	{
		iterationsField->setString(std::to_string(previousView.iterations));
		nextView = previousView;
	}

	if (resetButton->clicked())
	{
		iterationsField->setString(std::to_string(defaultView.iterations));
		nextView = defaultView;
	}

	if (generateButton->clicked())
	{
		nextView.color = colorScheme->getState();
		nextView.iterations = iterationsField->getValueInt();

		previousView = std::move(currentView);
		currentView = std::move(nextView);

		isComputed = false;
		computing = std::thread(&Mandelbrot::compute, this, currentView);
		computing.detach();

		for (size_t i = 0; i < 5; i++)
		{
			radiusFrame[i].position = sf::Vector2f(0, 0);
		}
	}
}


void Mandelbrot::draw()
{
	window.clear(sf::Color::White);

	for (auto& i : buttons)
	{
		i->draw(window);
	}

	for (auto& i : stateButtons)
	{
		i->draw(window);
	}

	for (auto& i : textFields)
	{
		i->draw(window);
	}

	window.draw(resultSprite);
	window.draw(centerText);
	window.draw(radiusText);
	window.draw(radiusFrame);

	window.display();
}

void Mandelbrot::compute(View & settings)
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
				double x_new = pow(x, 2) - pow(y, 2) + c_re;
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

Mandelbrot::~Mandelbrot()
{
}
