//
// Created by alex on 09.05.17.
//

#include "client.hpp"

client::client(sf::RenderWindow& window) :
		graphics(window),
		key_move(0),
		key_action(0)
{

}

void client::connect(sf::IpAddress ip) {
	socket.connect(ip, 2000);
	std::cout << "Connected with server" << std::endl;

	socket.receive(INPUT);

	INPUT >> my_number;
	std::cout << "my number is: " << my_number << std::endl;
	graphics.draw_wait_con();
}

void client::run(sf::RenderWindow& window){
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		key_move = 1;

	// костыльно! TODO : переписать
	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		key_move = 0;
		key_action = 0;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		key_move = 2;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		key_action = 1;

	// std::cout << "key_move: " << key_move << ", key_action: " << key_action << std::endl;
	OUTPUT << my_number << key_move << key_action;
	socket.send(OUTPUT);
	OUTPUT.clear();

	dt = clock.getElapsedTime().asSeconds();
	clock.restart();

	if (socket.receive(INPUT) == sf::Socket::NotReady) {
		graphics.draw(
				from_server.player_bottom_coords,
				from_server.player_top_coords,
				from_server.ball_coords + from_server.ball_speed * dt,
				-1,
				from_server.connection,
				from_server.lives_bottom,
				from_server.lives_top,
				from_server.score_bottom,
				from_server.score_top
		);
	} else {
		// std::cout << "Packet recieved"<< std::endl;
		INPUT >> from_server;



		std::cout << "player bottom: (" << from_server.player_bottom_coords.x << ", " << from_server.player_bottom_coords.y << ")" << std::endl;
		std::cout << "player top: (" << from_server.player_top_coords.x << ", " << from_server.player_top_coords.y << ")" << std::endl;
		std::cout << "ball: (" << from_server.ball_coords.x << ", " << from_server.ball_coords.y << ")" << std::endl;

		graphics.draw(
				from_server.player_bottom_coords,
				from_server.player_top_coords,
				from_server.ball_coords,
				from_server.broken_block,
				from_server.connection,
				from_server.lives_bottom,
				from_server.lives_top,
				from_server.score_bottom,
				from_server.score_top
		);

	}

	sleep(sf::milliseconds(10));
}

void client::disconnect() {
	socket.disconnect();
}

client::~client() {
	socket.disconnect();
}

sf::Packet& operator>>(sf::Packet& packet, data_from_server& to) {
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
