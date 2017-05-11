//
// Created by alex on 09.05.17.
//

#ifndef AW_SERVER_SERVER_HPP
#define AW_SERVER_SERVER_HPP

#include <SFML/Network.hpp>
#include "logic.hpp"
#include "../../common.hpp"

class server {
private:
	logic_world serv_world;

	sf::TcpListener listener;
	sf::TcpSocket socket_1; // программный интерфейс для обеспечения обмена данными между процессами
	sf::TcpSocket socket_2;

    // Для осуществления пакетной передачи дынных
	sf::Packet input_packet;
	sf::Packet output_packet;

	data_from_client data;
	data_from_server to_client;

public:
	server(const unsigned short port);
	~server();

	void input();
	void output();
};

#endif //AW_SERVER_SERVER_HPP
