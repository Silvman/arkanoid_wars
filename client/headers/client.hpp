//
// Created by alex on 09.05.17.
//

#ifndef ARKANOIDWARS_CLIENT_HPP
#define ARKANOIDWARS_CLIENT_HPP

#include <SFML/Network.hpp>
#include "graphics_scene.hpp"
#include "../../common.hpp"


class client {
private:
	sf::Clock clock;
	float dt;

	unsigned my_number;
	bool connection;
	unsigned int key_move, key_action;

	graphics_scene graphics;

	sf::TcpSocket socket;//программный интерфейс для обеспечения обмена данными между процессами
	sf::Packet INPUT;	//Для осуществления пакетной передачи дынных
	sf::Packet OUTPUT;

	data_from_server from_server;

	bool playerKicked;

public:
	client(sf::RenderWindow& window);
	void connect (sf::IpAddress ip) ;
	void run(sf::RenderWindow& window);
	void disconnect() ;
	~client() ;
	friend sf::Packet& operator >> (sf::Packet& packet, data_from_server& to);
};


#endif //ARKANOIDWARS_CLIENT_HPP
