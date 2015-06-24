/*
 * Room.h
 *
 *  Created on: 09.06.2015
 *      Author: Thomas Schlott
 */

#ifndef __ROOM_H__
#define __ROOM_H__

#include <vector>
#include <SFML\Network.hpp>

class Room
{
public:
	enum Direction : int
	{
		NORTH	= 0,
		EAST	= 1,
		SOUTH	= 2,
		WEST	= 3,
		UNDEFINED = 4
	};

private: //members
	int	m_x;
	int m_y;
	bool	exits[4];	
	Room*	neighbors[4];

public: //functions
	Room(int x, int y);
	~Room();

	//SETTER
	void setExit(bool exit, const int dir) { if(dir < 4) { exits[dir] = exit; } }
	void setNeighbor(Room* const neighbor, const int dir) { if(dir < 4) { neighbors[dir] = neighbor; } }

	//GETTER
	bool	hasExit(const int dir) const { return dir < 4 ? exits[dir] : false; }
	Room*	getNeighbor(const int dir) const { return dir < 4 ?  neighbors[dir] : nullptr; }
	int		getX() const { return m_x; }
	int		getY() const { return m_y; }
};

class Maze
{
private: //members
	Room *currentRoom;
	sf::TcpSocket *socket;
	
public: //members
	std::vector<Room*> rooms;
	
public: //methods
	Maze(Room* initRoom, sf::TcpSocket *s); /*constructor*/
	~Maze();
	void	addRoom(Room* const room); /*add Room to Maze*/
	void	connectRooms(Room *thisRoom, int const dir); /*connect two neighbour rooms (symmetry)*/
	Room*	getCurrentRoom(){return currentRoom;} /*returns current room of pi*/
	bool	move(int dir);
	
};

#endif//__ROOM_H__