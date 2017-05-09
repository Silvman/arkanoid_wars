//
// Created by silvman on 09.05.17.
//

#ifndef SERVER_H
#define SERVER_H

#include <SFML/Network.hpp>
#include <iostream>
#include "logic.h"

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


#endif //SERVER_H
