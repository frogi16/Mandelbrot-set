#pragma once

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

	bool View::operator==(const View &b) const { return (resolution == b.resolution && iterations == b.iterations && radius == b.radius && center == b.center &&color == b.color); }
	bool View::operator!=(const View &b) const { return !operator==(b); }

	int resolution{ 768 }, iterations{ 128 };
	double radius{ 2 };		//half of the side of the square
	sf::Vector2<double> center{ -0.5, 0 };
	char color{ 'R' };

	std::string getBasicString() const { return std::string{ std::to_string(radius) + " " + std::to_string(center.x) + " " + std::to_string(center.y) + "\n" }; }	//info required to computations
	std::string getFullString() const { return getBasicString() + std::to_string(resolution) + " " + std::to_string(iterations) + " " + color; }	//info about the presentation of result
};

struct ViewRepresentation
{
	View data;
	std::string name = {};
	sf::Texture previewTexture = {};
	sf::Sprite previewSprite = {};
	sf::Text nameText = {};
	sf::IntRect dimensions = {};
};