#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

#include "Viewer.h"
using namespace std;
#define TEXTURE_SIZE 75
#define TEXTURE_SIZE_2 37.5
#define MIDDLE_PIX 225

Viewer::Viewer(Maze *maze)
{
	m_maze = maze;
	window = new sf::RenderWindow(sf::VideoMode(525, 525), "Robo Zuul");
	m_texArray[UNEXPLORED].loadFromFile("unerkundet.png");
	m_texArray[L_PIECE].loadFromFile("2exits.png");
	m_texArray[LINE_PIECE].loadFromFile("durchgang.png");
	m_texArray[CROSS_PIECE].loadFromFile("4exits.png");
	m_texArray[T_PIECE].loadFromFile("3exits.png");
	m_texArray[DEAD_END].loadFromFile("sackgasse.png");
	m_texArray[FULLBLOCK].loadFromFile("vollblock.png");

	roboTex.loadFromFile("robo.png");
	roboSprite.setTexture(roboTex);
	roboSprite.setPosition(MIDDLE_PIX, MIDDLE_PIX);
}

Viewer::~Viewer()
{
	delete window;
}

int Viewer::getTextureForRoom(Room &r, int *rotation)
{
	int ret = UNEXPLORED;
	bool south = r.hasExit(Room::SOUTH);
	bool east = r.hasExit(Room::EAST);
	bool west = r.hasExit(Room::WEST); 
	bool north = r.hasExit(Room::NORTH);

	char exitcount = 0;

	for(char i = 0; i < 4; ++i)
	{
		if(r.hasExit(i))
		{
			exitcount++;
		}
	}

	switch(exitcount)
	{
	case 0:	ret = FULLBLOCK;	break; //no exits
	case 1:	ret = DEAD_END;
		if(east)
		{
			*rotation = 90;
		}
		if(south)
		{
			*rotation = 180;
		}
		if(west)
		{
			*rotation = 270;
		}
		break; //deadend
	case 2:	ret = LINE_PIECE;
		if(south && north)
		{
			ret = LINE_PIECE;
		}
		else if(east && west)
		{
			ret = LINE_PIECE;
			*rotation = 90;
		}
		else //L-curve
		{
			ret = L_PIECE;

			if(north && west)
			{
				*rotation = 270;
			}
			else if(west && south)
			{
				*rotation = 180;
			}
			else if(south && east)
			{
				*rotation = 90;
			}
			else if(east && north)
			{
				//no rotation
			}
		}
		break; //walkthrough
	case 3:	ret = T_PIECE;
		if(!north)
		{
			*rotation = 90;
		}
		else if(!east)
		{
			*rotation = 180;
		}
		else if(!south)
		{
			*rotation = 270;
		}
		break; //3 exits
	case 4:	ret = CROSS_PIECE;	break; //4 exits
	default:					break; //default
	}
	return ret;
}

void Viewer::buildView()
{
	bool found = true;
	bool keyPressed = true;
	int x, y, i, rotation;
	int centerX, absolutX;
	int centerY, absolutY;
	Room::Direction dir;
	sf::Sprite spriteArray[49];

	i = 0;
	for(y = 0; y < 7; ++y)
	{
		for(x = 0; x < 7; ++x) /*set position on window for all 49 sprites*/
		{
			spriteArray[i].setOrigin(TEXTURE_SIZE_2, TEXTURE_SIZE_2);
			spriteArray[i].setPosition(
				TEXTURE_SIZE_2 + TEXTURE_SIZE * x,
				TEXTURE_SIZE_2 + TEXTURE_SIZE * y);
			++i;
		}
	}


	while (window->isOpen())
	{
		//Refresh Vars
		centerX = m_maze->getCurrentRoom()->getX();
		centerY = m_maze->getCurrentRoom()->getY();

		//build view:
		i = 0;
		for(y = -3; y < 4; ++y)
		{
			absolutY = centerY + y;
			for(x = -3; x < 4; ++x)
			{
				absolutX = centerX + x;
				found = false;
				for(Room *room : m_maze->rooms) /*set respective texture for all sprites*/
				{
					if( room->getX() == absolutX &&
						room->getY() == absolutY)
					{
						rotation = 0;
						spriteArray[i].setRotation(0);
						spriteArray[i].setTexture(m_texArray[getTextureForRoom(*room, &rotation)], false);
						spriteArray[i].rotate(rotation);
						found = true;
						break;
					}
				}
				if(!found) /*if room isn't explored yet on coordinate x/y. (not yet in maze)*/
				{
					spriteArray[i].setRotation(0);
					spriteArray[i].setTexture(m_texArray[UNEXPLORED], false);
				}
				++i;
			}
		}

		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window->close();
		}

		window->clear(sf::Color(255, 255, 255));

		for(i = 0; i < 49; ++i)
		{
			window->draw(spriteArray[i]);
		}
		window->draw(roboSprite);
		window->display();
		dir = ListenKeys();
		if(dir == Room::UNDEFINED) 
		{
			keyPressed = false;
		}
		else /*user pressed an arrow key. now only possible for one time until pi moves in real life*/
		{
			if(!keyPressed)
			{
				m_maze->move(dir);
				keyPressed = true;
			}
		}
	}
}

Room::Direction Viewer::ListenKeys()
{
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		return Room::NORTH;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		return Room::WEST;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		return Room::EAST;
	}
	else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		return Room::SOUTH;
	}

	return Room::UNDEFINED;
}

