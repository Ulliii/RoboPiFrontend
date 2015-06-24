#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <SFML\Graphics.hpp>
#include "Room.h"
class Viewer
{
private: //members
	static const int UNEXPLORED = 0;
	static const int DEAD_END = 1;
	static const int L_PIECE = 2;
	static const int LINE_PIECE = 3;
	static const int T_PIECE = 4;
	static const int CROSS_PIECE = 5;
	static const int FULLBLOCK = 6;

	Maze *m_maze;

	sf::RenderWindow *window;
	sf::Texture m_texArray[7];
	sf::Texture roboTex;
	sf::Sprite roboSprite;
public: //methods
	Viewer(Maze *maze); /*constructor*/
	~Viewer();

	int getTextureForRoom(Room &r, int *rotation);
	void buildView();
	Room::Direction ListenKeys();
}; 

#endif//_VIEWER_H_