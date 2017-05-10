//
// Created by silvman on 10.05.17.
//

#include "common.hpp"

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
    packet << from.winner;

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

    int b;
    packet >> b;

    switch (b) {
        case 1: {
            to.winner = bottom;
            break;
        }

        case 2: {
            to.winner = top;
            break;
        }

        default:
        case 0: {
            to.winner = no;
            break;
        }
    }

    return packet;
}


sf::Packet& operator >> (sf::Packet& packet, players& a) {
    int b;
    packet >> b;

    switch (b) {
        case 1: {
            a = bottom;
            break;
        }

        case 2: {
            a = top;
            break;
        }

        default:
        case 0: {
            a = no;
            break;
        }
    }

    return packet;
}

sf::Packet& operator >> (sf::Packet& packet, num_move& a) {
    int b;
    packet >> b;

    switch (b) {
        case 1: {
            a = move_left;
            break;
        }

        case 2: {
            a = move_right;
            break;
        }

        default:
        case 0: {
            a = stay;
            break;
        }
    }

    return packet;
}

sf::Packet& operator >> (sf::Packet& packet, num_action& a) {
    int b;
    packet >> b;

    switch (b) {
        case 1: {
            a = push_vertical;
            break;
        }

        case 2: {
            a = push_left;
            break;
        }

        case 3: {
            a = push_right;
            break;
        }

        case 4: {
            a = push_lefter;
            break;
        }

        case 5: {
            a = push_righter;
            break;
        }

        case 6: {
            a = use_weapon;
            break;
        }

        default:
        case 0: {
            a = nothing;
            break;
        }
    }

    return packet;
}