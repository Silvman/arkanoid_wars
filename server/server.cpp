//
// Created by alex on 09.05.17.
//

#include "server.hpp"


// эта перегрузка нужна в сервере
sf::Packet& operator << (sf::Packet& packet, data_from_server& from) {
	packet << from.connection;
	packet << from.player_bottom_coords.x << from.player_bottom_coords.y;
	packet << from.player_top_coords.x << from.player_top_coords.y;
	packet << from.ball_coords.x << from.ball_coords.y;
	packet << from.ball_speed.x << from.ball_speed.y;
	packet << from.broken_block;
	packet << from.isPlayerKicked;
	packet << from.score_bottom << from.score_top << from.lives_bottom << from.lives_top;

	return packet;
}

// Эта перегрузка будет нужна в клиенте
sf::Packet& operator >> (sf::Packet& packet, data_from_server& to) {
	packet >> to.connection;
	packet >> to.player_bottom_coords.x >> to.player_bottom_coords.y;
	packet >> to.player_top_coords.x >> to.player_top_coords.y;
	packet >> to.ball_coords.x >> to.ball_coords.y;
	packet >> to.ball_speed.x >> to.ball_speed.y;
	packet >> to.broken_block;
	packet >> to.isPlayerKicked;
	packet >> to.score_bottom >> to.score_top >> to.lives_bottom >> to.lives_top;

	return packet;
}

// server
server::server(const unsigned short port) : serv_world(1024,768) {
	listener.listen(port);
	std::cout << "Server is listening to port " << port << ", waiting for connection 1. " << std::endl;

	listener.accept(socket_1);
	OUTPUT << 1;
	socket_1.send(OUTPUT);
	OUTPUT.clear();

	std::cout << "Client 1 is connected, ip " << socket_1.getRemoteAddress() << std::endl;

	std::cout << "Server is listening to port " << port << ", waiting for connection 2. " << std::endl;
	listener.accept(socket_2);
	OUTPUT << 2;
	socket_2.send(OUTPUT);
	OUTPUT.clear();

	std::cout << "Client 2 is connected, ip " << socket_2.getRemoteAddress() << std::endl;
}

void server::input() {
	// Серв должен принимать 4 значения-ключа - движение нижнего, действие нижнего, движение верхнего, действие верхнего
	// В движениях: 1 - влево, 2 - вправо, 0 - по-умолчанию
	// В действиях  1 - запуск шара

	if (socket_1.receive(INPUT) == sf::Socket::Done ) {
		data.bot_connect = 1;

		INPUT >> data.num >> data.key_bottom_move >> data.key_bottom_action;
		//std::cout << "Socket 1. Recieved: " << data.num << ", " << data.key_bottom_move <<
		//		", " << data.key_bottom_action << std::endl;

		INPUT.clear();

	} else data.bot_connect = 0;

	if(socket_2.receive(INPUT) == sf::Socket::Done) {
		data.top_connect = 1;

		INPUT >> data.num >> data.key_top_move >> data.key_top_action;
		//std::cout << "Socket 2. Recieved: " << data.num << ", " << data.key_top_move <<
		//		", " << data.key_top_action << std::endl;

		INPUT.clear();
	} else data.top_connect = 0;
}

void server::output() {
	if (data.bot_connect == 1 && data.top_connect == 1) {
		serv_world.update(data.key_bottom_move, data.key_bottom_action, data.key_top_move, data.key_top_action);

		to_client.player_bottom_coords  = serv_world.givePlayerBottomCoords();
		to_client.player_top_coords     = serv_world.givePlayerTopCoords();
		to_client.ball_coords           = serv_world.giveBallCoords();
		to_client.ball_speed            = serv_world.giveBallSpeed();
		to_client.broken_block          = serv_world.getBrokenBlocks();
		to_client.isPlayerKicked        = serv_world.checkPlayerKicked();
		to_client.score_bottom          = serv_world.giveScoreBottom();
		to_client.score_top             = serv_world.giveScoreTop();
		to_client.lives_bottom          = serv_world.giveLivesBottom();
		to_client.lives_top             = serv_world.giveLivesTop();

		connection = 1;
		to_client.connection = 1;

		OUTPUT << to_client;

		socket_1.send(OUTPUT);
		socket_2.send(OUTPUT);
		OUTPUT.clear();
	} else {
		to_client.connection = connection;

		OUTPUT << to_client;
	}

	if(data.bot_connect == 1 && data.top_connect == 0) {
		socket_1.send(OUTPUT);
		OUTPUT.clear();
	}

	if(data.bot_connect == 0 && data.top_connect == 1) {
		socket_2.send(OUTPUT);
		OUTPUT.clear();
	}

	connection = 0;

}

server::~server() {
	socket_1.disconnect();
	socket_2.disconnect();
	listener.close();
};
