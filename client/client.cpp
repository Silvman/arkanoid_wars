//
// Created by alex on 09.05.17.
//

#include "headers/client.hpp"


client::client(sf::RenderWindow& window) :
		graphics(window),
		key_move(stay),
		key_action(nothing)
{
    // socket.setBlocking(false);
}

void client::connect(sf::IpAddress ip) {
	socket.connect(ip, 2000);
	std::cout << "Connected with server" << std::endl;

	socket.receive(input_packet);

	input_packet >> my_number;
	std::cout << "my number is: " << my_number << std::endl;
	graphics.draw_wait_con();
}

int client::run(sf::RenderWindow& window){

	if (gameIsOver == 0) {

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			key_move = move_left;

		// костыльно! TODO : переписать
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			key_move = stay;
			key_action = nothing;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			key_move = move_right;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			key_action = push_vertical;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
			key_action = push_left;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			key_action = push_lefter;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
			key_action = push_right;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			key_action = push_righter;

		// std::cout << "key_move: " << key_move << ", key_action: " << key_action << std::endl;
		output_packet << my_number << key_move << key_action;

		socket.send(output_packet);
		output_packet.clear();


		dt = clock.getElapsedTime().asSeconds();
		clock.restart();

			if (socket.receive(input_packet) == sf::Socket::NotReady) {

				// отрисовка пока пакет не дошёл

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
				window.display();


			}
			else {
				// std::cout << "Packet recieved"<< std::endl;
				input_packet >> from_server;

				std::cout << "player bottom: (" << from_server.player_bottom_coords.x << ", "
						<< from_server.player_bottom_coords.y << ")" << std::endl;
				std::cout << "player top: (" << from_server.player_top_coords.x << ", "
						<< from_server.player_top_coords.y
						<< ")" << std::endl;
				std::cout << "ball: (" << from_server.ball_coords.x << ", " << from_server.ball_coords.y << ")"
						<< std::endl;

				playerKicked = from_server.isPlayerKicked;
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
				window.display();


				if (from_server.winner != no) {
					gameIsOver = 1;
					return 3;
				}
			}

			sleep(sf::milliseconds(10));
			return 3;
		} else {
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

			return graphics.draw_win(from_server.winner);
		}

}

void client::disconnect() {
	socket.disconnect();
}

client::~client() {
	socket.disconnect();
}