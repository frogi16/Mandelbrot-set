#include "Mandelbrot.h"

#include <complex>
#include <fstream>

Mandelbrot::Mandelbrot()
{
}

void Mandelbrot::init(int argc, char ** argv)
{
	if (argc == 4)		//direct parameters
	{
		currentView.center.x = std::stod(argv[1]);
		currentView.center.y = std::stod(argv[2]);
		currentView.radius = std::stod(argv[3]);
	}
	else if (argc == 2)		//path to file with parameters
	{
		std::fstream in(argv[1], std::ios_base::in);
		double temp;
		in >> temp;
		currentView.center.x = temp;
		in >> temp;
		currentView.center.y = temp;
		in >> temp;
		currentView.radius = temp;
	}

	init();
}

void Mandelbrot::init()
{
	desktop = sf::VideoMode::getDesktopMode();
	window.create(sf::VideoMode(desktop.width, desktop.height), "Mandelbrot", sf::Style::Fullscreen);

	graphDimensions = sf::IntRect(desktop.width - desktop.height, 0, desktop.height, desktop.height);
	settingsDimensions = sf::IntRect(0, 0, desktop.width - desktop.height - 2, 400);
	currentView.resolution = graphDimensions.width;

	result.create(currentView.resolution, currentView.resolution, sf::Color::Black);
	resultSprite.setPosition(graphDimensions.left, graphDimensions.top);

	arial.loadFromFile("arial.ttf");

	viewExplorer = std::make_unique<ViewExplorer>(sf::IntRect(0, 400, settingsDimensions.width, desktop.height - 400));

	undoButton = std::make_shared<Button>(sf::IntRect(220, 100, 120, 40), "Undo");
	resetButton = std::make_shared<Button>(sf::IntRect(360, 100, 120, 40), "Reset");
	exportButton = std::make_shared<Button>(sf::IntRect(220, 160, 120, 40), "Export");
	generateButton = std::make_shared<Button>(sf::IntRect(360, 160, 120, 40), "Generate");
	buttons.push_back(undoButton);
	buttons.push_back(resetButton);
	buttons.push_back(exportButton);
	buttons.push_back(generateButton);

	colorScheme = std::make_shared<StateButton>(sf::IntRect(160, 100, 40, 40));
	iterationsField = std::make_shared<TextField>(sf::IntRect(20, 100, 120, 40), FieldType::Number, "128", 1);
	resolutionField = std::make_shared<TextField>(sf::IntRect(20, 160, 120, 40), FieldType::Number, std::to_string(graphDimensions.width), 1);
	filenameField = std::make_shared<TextField>(sf::IntRect(20, 220, 460, 40), FieldType::WindowsFilename, "", 1);
	stateButtons.push_back(colorScheme);
	textFields.push_back(iterationsField);
	textFields.push_back(resolutionField);
	textFields.push_back(filenameField);

	iterationsTitle = std::make_shared<sf::Text>("Iterations:", arial, 16);
	iterationsTitle->setColor(sf::Color::Black);
	iterationsTitle->setPosition(20, 80);
	texts.push_back(iterationsTitle);

	resolutionTitle = std::make_shared<sf::Text>("Resolution:", arial, 16);
	resolutionTitle->setColor(sf::Color::Black);
	resolutionTitle->setPosition(20, 140);
	texts.push_back(resolutionTitle);

	colorsTitle = std::make_shared<sf::Text>("Color:", arial, 16);
	colorsTitle->setColor(sf::Color::Black);
	colorsTitle->setPosition(160, 80);
	texts.push_back(colorsTitle);

	filenameTitle = std::make_shared<sf::Text>("Filename:", arial, 16);
	filenameTitle->setColor(sf::Color::Black);
	filenameTitle->setPosition(20, 200);
	texts.push_back(filenameTitle);

	loading = std::make_shared<sf::Text>("...", arial, 48);
	loading->setColor(sf::Color::Black);
	loading->setPosition(800, 120);

	centerText.setFont(arial);
	centerText.setColor(sf::Color::Black);
	centerText.setPosition(0, 0);
	centerText.setCharacterSize(24);

	radiusText.setFont(arial);
	radiusText.setColor(sf::Color::Black);
	radiusText.setPosition(0, 30);
	radiusText.setCharacterSize(24);

	previousView = currentView;
	defaultView = currentView;		//needed to reset settings
	nextView = currentView;
	startThread();

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
			resultSprite.setScale(graphDimensions.height / resultSprite.getLocalBounds().height, graphDimensions.height / resultSprite.getLocalBounds().height);
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

	viewExplorer->handleMouse(mouse);
}

void Mandelbrot::handleClicks()
{
	if (undoButton->clicked())
	{
		iterationsField->setString(std::to_string(previousView.iterations));
		std::swap(currentView, previousView);

		adjustResolution();
		startThread();
		clearFrame();
	}

	if (resetButton->clicked())
	{
		iterationsField->setString(std::to_string(defaultView.iterations));
		previousView = std::move(currentView);
		currentView = defaultView;

		adjustResolution();
		startThread();
		clearFrame();
	}

	if (generateButton->clicked())
	{
		nextView.color = colorScheme->getState();
		nextView.iterations = iterationsField->getValueInt();
		previousView = std::move(currentView);
		currentView = std::move(nextView);

		adjustResolution();
		startThread();
		clearFrame();
	}

	if (exportButton->clicked())
	{
		if (!isComputing)
		{
			isComputing = true;
			exportView(filenameField->getValueString()+".txt");
			exporting = std::thread(&Mandelbrot::exportImage, this, filenameField->getValueString() + ".png");
			exporting.detach();
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

	for (auto& i : texts)
	{
		window.draw(*i);
	}

	if (isComputing)
		window.draw(*loading);
	
	viewExplorer->draw(window);
	window.draw(resultSprite);
	window.draw(centerText);
	window.draw(radiusText);
	window.draw(radiusFrame);

	window.display();
}

void Mandelbrot::compute(const View & settings)
{
	int iMax = settings.iterations;
	double unit = 2 * settings.radius / (double)settings.resolution;
	
	for (int stepY = 0; stepY < settings.resolution; stepY++)
	{
		for (int stepX = 0; stepX < settings.resolution; stepX++)
		{
			//implementation written below proved to be ridiculously slow despite the use of standard complex type so I changed it to less elegant, but much more effective simple doubles arithmetic
			//for default view settings it took about 6.13 seconds to compute all steps using complex type, whereas doubles needed 2.42 seconds on my computer (1 thread computing, debug mode)
			//left for further analysis

			/*std::complex<double> z{ 0, 0 };
			std::complex<double> c{ settings.center.x - settings.radius + stepX*unit, settings.center.y - settings.radius + stepY*unit };
			int i = 0;

			while (pow(z.real(), 2) + pow(z.imag(), 2) <= 4 && i < iMax)
			{
				double z_square = pow(z.real(), 2) - pow(z.imag(), 2) + c.real();

				z.imag(2 * z.real()*z.imag() + c.imag());
				z.real(z_square);
				i++;
			}*/

			double z_x = 0;
			double z_y = 0;
			double c_x = settings.center.x - settings.radius + stepX*unit;
			double c_y = settings.center.y - settings.radius + stepY*unit;
			int i = 0;

			while (pow(z_x, 2) + pow(z_y, 2) <= 4 && i < iMax)
			{
				double z_square = pow(z_x, 2) - pow(z_y, 2) + c_x;

				z_y = 2 * z_x*z_y + c_y;
				z_x = z_square;
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
	isComputing = false;
}

void Mandelbrot::adjustResolution()
{
	if (resolutionField->getValueInt() != currentView.resolution)
	{
		currentView.resolution = resolutionField->getValueInt();
		result.create(currentView.resolution, currentView.resolution, sf::Color::Black);
	}
}

void Mandelbrot::startThread()
{
	if (!isComputing)
	{
		isComputing = true;
		isComputed = false;
		computing = std::thread(&Mandelbrot::compute, this, currentView);
		computing.detach();
	}
}

void Mandelbrot::clearFrame()
{
	for (size_t i = 0; i < 5; i++)
	{
		radiusFrame[i].position = sf::Vector2f(0, 0);
	}
}

void Mandelbrot::exportView(std::string filename) const
{
	std::fstream out(filename, std::ios::app);
	out << currentView.getFullString();
	out.close();
}

void Mandelbrot::exportImage(std::string filename)
{
	result.saveToFile(filename);
	isComputing = false;
}

Mandelbrot::~Mandelbrot()
{
}
