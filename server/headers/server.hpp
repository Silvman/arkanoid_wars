//
// Created by alex on 09.05.17.
//

#ifndef AW_SERVER_SERVER_HPP
#define AW_SERVER_SERVER_HPP


#include <SFML/Network.hpp>
#include "logic.hpp"
#include "../../common.hpp"

struct DATA {
	unsigned int key_bottom_move;
	unsigned int key_bottom_action;
	unsigned int key_top_move;
	unsigned int key_top_action;
	bool bot_connect;
	bool top_connect;

	unsigned num;
};

class server {
private:
	logic_world serv_world;

	sf::TcpListener listener;
	sf::TcpSocket socket_1; //программный интерфейс для обеспечения обмена данными между процессами
	sf::TcpSocket socket_2;

	sf::Packet INPUT;
	sf::Packet OUTPUT;
	bool connection;
	//Для осуществления пакетной передачи дынных

	DATA data;

	data_from_server to_client;
	/* unsigned int key_bottom_move;
	 unsigned int key_bottom_action;
	 unsigned int key_top_move;
	 unsigned int key_top_action;

	 unsigned num;
 */

public:
	server(const unsigned short port);
	~server();

	void input();
	void output();
};




#endif //AW_SERVER_SERVER_HPP
