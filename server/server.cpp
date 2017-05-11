//
// Created by alex on 09.05.17.
//

#include "headers/server.hpp"

// server
server::server(const unsigned short port) : game_over(false) {
    // socket_1.setBlocking(false);
    // socket_2.setBlocking(false);

	listener.listen(port);
	std::cout << "Server is listening to port " << port << ", waiting for connection 1. " << std::endl;

	listener.accept(socket_1);
	output_packet << 1;
	socket_1.send(output_packet);
	output_packet.clear();

	std::cout << "Client 1 is connected, ip " << socket_1.getRemoteAddress() << std::endl;

	std::cout << "Server is listening to port " << port << ", waiting for connection 2. " << std::endl;
	listener.accept(socket_2);
	output_packet << 2;
	socket_2.send(output_packet);
	output_packet.clear();

    serv_world = new logic_world(gc_window_size_x, gc_window_size_y);

	std::cout << "Client 2 is connected, ip " << socket_2.getRemoteAddress() << std::endl;
}

void server::input() {
	// Серв должен принимать 4 значения-ключа - движение нижнего, действие нижнего, движение верхнего, действие верхнего
	// В движениях: 1 - влево, 2 - вправо, 0 - по-умолчанию
	// В действиях  1 - запуск шара
    if (!game_over) {
        if (socket_1.receive(input_packet) == sf::Socket::Done) {
            data.bot_connect = 1;

            input_packet >> data.num >> data.key_bottom_move >> data.key_bottom_action;
            //std::cout << "Socket 1. Recieved: " << data.num << ", " << data.key_bottom_move <<
            //		", " << data.key_bottom_action << std::endl;

            input_packet.clear();

        } else data.bot_connect = 0;

        if (socket_2.receive(input_packet) == sf::Socket::Done) {
            data.top_connect = 1;

            input_packet >> data.num >> data.key_top_move >> data.key_top_action;
            //std::cout << "Socket 2. Recieved: " << data.num << ", " << data.key_top_move <<
            //		", " << data.key_top_action << std::endl;

            input_packet.clear();
        } else data.top_connect = 0;
    } else {
        bool replay_1 = false, replay_2 = false;
		std::cout << "Wait" << std::endl;
		
        if (socket_1.receive(input_packet) == sf::Socket::Done) {
            input_packet >> replay_1;
			std::cout << replay_1 << std::endl;
        }
        if (socket_2.receive(input_packet) == sf::Socket::Done) {
            input_packet >> replay_2;
			std::cout << replay_2 << std::endl;
        }
        
        if(replay_1 && replay_2) {
            game_over = false;
            replay();
        }
    }
}

void server::output() {
	if (data.bot_connect == 1 && data.top_connect == 1) {
		serv_world->update(data.key_bottom_move, data.key_bottom_action, data.key_top_move, data.key_top_action);

		if (serv_world->whoWin() != no) {
			game_over = true;
            //std::cout << "Winner: " << serv_world->whoWin() << std::endl;
		}
        
        std::cout << "Working" << std::endl;

		to_client.player_bottom_coords  = serv_world->givePlayerBottomCoords();
		to_client.player_top_coords     = serv_world->givePlayerTopCoords();
		to_client.ball_coords           = serv_world->giveBallCoords();
		to_client.ball_speed            = serv_world->giveBallSpeed();
		to_client.broken_block          = serv_world->getBrokenBlocks();
		to_client.isPlayerKicked        = serv_world->checkPlayerKicked();
		to_client.score_bottom          = serv_world->giveScoreBottom();
		to_client.score_top             = serv_world->giveScoreTop();
		to_client.lives_bottom          = serv_world->giveLivesBottom();
		to_client.lives_top             = serv_world->giveLivesTop();
        to_client.winner                = serv_world->whoWin();

		to_client.connection = 1;

		output_packet << to_client;

		socket_1.send(output_packet);
		socket_2.send(output_packet);
		output_packet.clear();
	} else
		output_packet << to_client;


	if(data.bot_connect == 1 && data.top_connect == 0) {
		socket_1.send(output_packet);
		output_packet.clear();
	}

	if(data.bot_connect == 0 && data.top_connect == 0) {
		//удаление физической игры и игры в целом
	}

	if(data.bot_connect == 0 && data.top_connect == 1) {
		socket_2.send(output_packet);
		output_packet.clear();
	}

	to_client.connection = 0;

}

void server::replay() {
    if (serv_world) {
        delete serv_world;
        serv_world = new logic_world(gc_window_size_x, gc_window_size_y);
        game_over = false;
    }
}

server::~server() {
    delete serv_world;
	socket_1.disconnect();
	socket_2.disconnect();
	listener.close();
};
