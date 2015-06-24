#include <iostream>
#include "Room.h"

#define BUF_NORTH	110
#define BUF_EAST	111
#define BUF_SOUTH	115
#define BUF_WEST	119

Room::Room(int x, int y)
{
	m_x = x;
	m_y = y;
	for(int i = 0; i < 4; ++i)
	{
		exits[i] = false;
		neighbors[i] = nullptr;
	} 
}
Room::~Room()
{

}
Maze::Maze(Room* initRoom,  sf::TcpSocket *s)
{
	socket = s;
	currentRoom = initRoom;
}
Maze::~Maze()
{
	for(Room *room : rooms)
	{
		delete room;
	}
}
void Maze::addRoom(Room* const room)
{
	int rX = room->getX();
	int rY = room->getY();
	bool isNew = true;
	for(Room *r : rooms)
	{
		if(r->getX() == rX && r->getY() == rY)
		{
			isNew = false;
		}
	}
	//Connect rooms
	if(isNew)
	{
		for (Room *otherRoom : rooms) /*access by pointer to avoid copying*/
		{
			if(otherRoom->getY() == rY) /*room is in same column (above or beneath)*/
			{
				if(otherRoom->getX() == rX - 1)
				{
					room->setNeighbor(otherRoom, Room::WEST);
					otherRoom->setNeighbor(room, Room::EAST);
					connectRooms(room, Room::EAST);
				}
				else if(otherRoom->getX() == rX + 1)
				{
					room->setNeighbor(otherRoom, Room::EAST);
					otherRoom->setNeighbor(room, Room::WEST);
					connectRooms(room, Room::WEST);
				}
			}
			else if(otherRoom->getX() == rX) /*room is in same line (left or right of it)*/
			{
				if(otherRoom->getY() == rY - 1)
				{
					room->setNeighbor(otherRoom, Room::NORTH);
					otherRoom->setNeighbor(room, Room::SOUTH);
					connectRooms(room, Room::SOUTH);
				}
				else if(otherRoom->getY() == rY + 1)
				{
					room->setNeighbor(otherRoom, Room::SOUTH);
					otherRoom->setNeighbor(room, Room::NORTH);
					connectRooms(room, Room::NORTH);
				}
			}
		}
		
		rooms.push_back(room); /*add this room to maze*/
	}
}
void Maze::connectRooms(Room *thisRoom, int const dir)
{
	if(dir > 4)
		return;

	Room* neighbor;
	for(Room *room : rooms)
	{
		if( room->getX() == thisRoom->getX() &&
			room->getY() == thisRoom->getY()) /*find room in maze*/
		{
			room->setExit(true, dir);
			neighbor = room->getNeighbor(dir); /*find neighbour next to room in coordination system and connect both*/
			if(neighbor != nullptr)
			{
				switch(dir)
				{
				case Room::NORTH:	neighbor->setExit(true, Room::SOUTH);	break;
				case Room::EAST:	neighbor->setExit(true, Room::WEST);	break;
				case Room::SOUTH:	neighbor->setExit(true, Room::NORTH);	break;
				case Room::WEST:	neighbor->setExit(true, Room::EAST);	break;
				}
				break;
			}

		}
	}
}
bool Maze::move(int dir)
{
	bool ret = false;
	char buffer[64];
	memset(buffer, 0, 64);
	std::size_t received = 0;
	Room *newRoom;
	std::string message;

	if(currentRoom->hasExit(dir))
	{
		switch(dir) /*set new Room with respective coordinates*/
		{
		case Room::NORTH: 	newRoom = new Room(currentRoom->getX(),		currentRoom->getY() - 1);	break;
		case Room::EAST: 	newRoom = new Room(currentRoom->getX() + 1,	currentRoom->getY());		break;
		case Room::SOUTH:	newRoom = new Room(currentRoom->getX(),		currentRoom->getY() + 1);	break;
		case Room::WEST: 	newRoom = new Room(currentRoom->getX() - 1,	currentRoom->getY());		break;
		}

		switch(dir) /*"encode" command for pi to move.*/
		{
		case 0:	message = "north";	break;
		case 1:	message = "east";	break;
		case 2:	message = "south";	break;
		case 3:	message = "west";	break;
		}
		socket->send(message.c_str(), message.size() + 1); /*send command to move in <dir>*/
		socket->receive(buffer, sizeof(buffer), received); /*wait until receiving exits of new room...*/


		std::string *strBuf = new std::string(buffer);
		if(strBuf->find("north") != std::string::npos) /*find all exits and set it to newRoom...*/
		{
			newRoom->setExit(true, 0);
		}
		if(strBuf->find("east") != std::string::npos)
		{
			newRoom->setExit(true, 1);
		}
		if(strBuf->find("south") != std::string::npos)
		{
			newRoom->setExit(true, 2);
		}
		if(strBuf->find("west") != std::string::npos)
		{
			newRoom->setExit(true, 3);
		}
		delete strBuf;

		addRoom(newRoom); /*Done! add newRoom to maze!*/
		currentRoom = currentRoom->getNeighbor(dir); /*pi went for a walk and is now in a (probable) "newRoom"*/
		ret = true;
	}
	return ret;
}
