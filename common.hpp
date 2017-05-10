//
// Created by silvman on 10.05.17.
//

#ifndef ARKANOIDWARS_COMMON_HPP
#define ARKANOIDWARS_COMMON_HPP

#include <SFML/Network.hpp>

enum num_move {
    stay = 0,
    move_left,
    move_right,
};

enum num_action {
    nothing = 0,
    push_vertical = 1,
    push_left,
    push_right,
    push_lefter,
    push_righter,
    use_weapon
};


enum players {
    no = 0,
    bottom,
    top
};

typedef struct data_from_server {
    bool connection;
    sf::Vector2f player_bottom_coords, player_top_coords, ball_coords, ball_speed;
    int broken_block;
    bool isPlayerKicked;
    int score_bottom, score_top, lives_bottom, lives_top;
    players winner;
} data_from_server;


typedef struct data_from_client {
    num_move key_bottom_move;
    num_action key_bottom_action;
    num_move key_top_move;
    num_action key_top_action;

    bool bot_connect;
    bool top_connect;

    unsigned num;
} data_from_client;

// эта перегрузка нужна в сервере
sf::Packet& operator << (sf::Packet& packet, data_from_server& from);

// Эта перегрузка будет нужна в клиенте
sf::Packet& operator >> (sf::Packet& packet, data_from_server& to);



sf::Packet& operator >> (sf::Packet& packet, num_move& a);

sf::Packet& operator >> (sf::Packet& packet, num_action & a);

#endif //ARKANOIDWARS_COMMON_HPP
