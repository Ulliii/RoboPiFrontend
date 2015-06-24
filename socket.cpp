/*
* SocketTCP.cpp
*
*  Created on: 20.05.2015
*      Author: Ulli
*/
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <conio.h>

#include "Viewer.h"
#include "Room.h"
//socket for client. ask for exits (direction) in next room.

int main(int argc, char** argv)
{
	
	if(argc != 3)
	{
		return 0;
	}
	
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect(argv[1], std::stoi(argv[2]));
	if (status != sf::Socket::Done)
	{
		std::cerr << "could not establish connection!" << std::endl;
		// error...
	}

	std::string message =  "look"; //get exits of initial rooms
	char buffer[64];
	memset(buffer, 0, 64);
	std::size_t received = 0;
	socket.send(message.c_str(), message.size() + 1);
	//socket.send(myPack);
	socket.receive(buffer, sizeof(buffer), received);

	Room *r0 = new Room(0, 0);						//initialize first room...
	std::string *strBuf = new std::string(buffer);
	if(strBuf->find("north") != std::string::npos)
	{
		r0->setExit(true, 0);
	}
	if(strBuf->find("east") != std::string::npos)
	{
		r0->setExit(true, 1);
	}
	if(strBuf->find("south") != std::string::npos)
	{
		r0->setExit(true, 2);
	}
	if(strBuf->find("west") != std::string::npos)
	{
		r0->setExit(true, 3);
	}
	delete strBuf;

	Maze *m = new Maze(r0, &socket);
	m->addRoom(r0);

	Viewer v(m);
	v.buildView(); //build gui


	delete m;
	socket.disconnect();


	/*Room *r1 = new Room(0,-1);
	Room *r2 = new Room(0, 1);
	Room *r3 = new Room(1, 1);	
	m->addRoom(r0);
	m->addRoom(r1);
	m->addRoom(r2);
	m->addRoom(r3);

	m->connectRooms(r0, Room::SOUTH);
	m->connectRooms(r0, Room::EAST);

	Viewer v(m);
	v.buildView(); //build "gui"

	delete m;
	socket.disconnect();*/

}


