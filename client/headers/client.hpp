//
// Created by alex on 09.05.17.
//

#ifndef ARKANOIDWARS_CLIENT_HPP
#define ARKANOIDWARS_CLIENT_HPP

#include <SFML/Network.hpp>
#include "graphics_scene.hpp"


class client {
private:
	sf::Clock clock;
	float dt;

	unsigned my_number;
	bool connection;

	num_move key_move;
    num_action key_action;

	graphics_scene graphics;

	sf::TcpSocket socket;//программный интерфейс для обеспечения обмена данными между процессами
	sf::Packet input_packet;	//Для осуществления пакетной передачи дынных
	sf::Packet output_packet;

	data_from_server from_server;

	bool playerKicked;
	bool gameIsOver = 0;

public:
	client(sf::RenderWindow& window);
	void connect (sf::IpAddress ip) ;
	int run(sf::RenderWindow& window);
	void disconnect();
	~client() ;
};


#endif //ARKANOIDWARS_CLIENT_HPP
