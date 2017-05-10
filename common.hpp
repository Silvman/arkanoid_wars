//
// Created by silvman on 10.05.17.
//

#ifndef ARKANOIDWARS_COMMON_HPP
#define ARKANOIDWARS_COMMON_HPP

#include <SFML/Network.hpp>

typedef struct data_from_server {
    bool connection;
    sf::Vector2f player_bottom_coords, player_top_coords, ball_coords, ball_speed;
    int broken_block;
    bool isPlayerKicked;
    int score_bottom, score_top, lives_bottom, lives_top;
} data_from_server;

// эта перегрузка нужна в сервере
sf::Packet& operator << (sf::Packet& packet, data_from_server& from);

// Эта перегрузка будет нужна в клиенте
sf::Packet& operator >> (sf::Packet& packet, data_from_server& to);

#endif //ARKANOIDWARS_COMMON_HPP
