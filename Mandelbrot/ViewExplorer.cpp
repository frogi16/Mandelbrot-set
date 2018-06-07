#include "ViewExplorer.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

ViewExplorer::ViewExplorer(sf::IntRect dimen) : dimensions(dimen), columns(3)
{
	loadViews();
	orderSprites();
}

void ViewExplorer::handleMouse(sf::Mouse mouse)
{
}

void ViewExplorer::draw(sf::RenderTarget & target)
{
	for (auto& i : represesentations)
	{
		target.draw(i.previewSprite);
	}
}

ViewExplorer::~ViewExplorer()
{
}

void ViewExplorer::loadViews()
{
	for (auto& i : std::experimental::filesystem::directory_iterator("."))		//dot means current directory. C++17 only, experimental approach, but I used old solutions and I'd rather avoid that
	{
		if (i.path().extension() == ".txt")
		{
			loadViewData(i.path().filename().generic_string());		//using these long namespaces and types is tiring and one day "experimental" namespace will be removed, so I converted it to standard string at the first opportunity. If implementation changes it will require changes only in this function (I hope)
		}
	}

	for (auto& i : std::experimental::filesystem::directory_iterator("."))		//unfortunately directory_iterator can access files in random order so I must search for connected images in the second loop
	{
		if (i.path().extension() == ".png")
		{
			loadViewImage(i.path().filename().generic_string());
		}
	}
}

void ViewExplorer::loadViewData(std::string filename)
{
	std::ifstream in(filename);
	std::stringstream buffer;
	buffer << in.rdbuf();	//apparently the most efficient way and works even under C++98

	std::vector<std::string> values;
	while (!buffer.eof())
	{
		std::string temp;
		buffer >> temp;
		values.push_back(std::move(temp));
	}
	values.pop_back();		//the last one is always empty. Probably can be more elegant. TODO

	ViewRepresentation temp{ View{values}, filename };
	represesentations.push_back(std::move(temp));
}

void ViewExplorer::loadViewImage(std::string filename)
{
	std::string name{ filename.erase(filename.back(), filename.back() - 4) };		//cut .png

	for (auto& i : represesentations)
	{
		if (i.name == name)		//found linked data
		{
			i.previewTexture.loadFromFile(filename);
		}
	}
}

void ViewExplorer::orderSprites()
{
	int previewWidth = dimensions.width / float(columns)*0.8f;
	int margin = (dimensions.width / float(columns) - previewWidth) * 2;
	float newScale;

	if (represesentations.size() > 0)
	{
		newScale = previewWidth / float(represesentations[0].previewSprite.getLocalBounds().width);
	}

	for (size_t i = 0; i < represesentations.size(); i++)
	{
		int actualColumn = i%columns + 1;
		int actualRow = i / columns + 1;
		represesentations[i].previewSprite.setTexture(represesentations[i].previewTexture);
		represesentations[i].previewSprite.setScale(newScale, newScale);
		represesentations[i].previewSprite.setPosition(margin*columns + previewWidth*columns, margin*actualRow + previewWidth*actualRow);
	}
}
