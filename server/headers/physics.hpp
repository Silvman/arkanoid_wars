//
// Created by silvman on 09.05.17.
//

#ifndef PHYSICS_H
#define PHYSICS_H

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "../../common.hpp"

#define PTM 30.0f
#define DEG 57.29577f

// физика
/*
 * TODO : физика
 *
 * Сделать так чтобы платформа упиралась в стенку, а не прходила через нее
 * Физика столкновения - чтобы кидал флаги об ударенных блоках
 *
 */

// каждому телу соответствует свой собственный id в зависимотси от рода
// 1 - платформа нижняя, 2 - платформа верхняя, 3 - шарик, 4 - блок, 5 - стенка

enum object_type {
    undef = 0,
    player_bottom_id = 1,
    player_top_id,
    ball_id,
    block_id,
    border_id
};

class physics_scene {
private:
    b2World world;

    // callback для соударений
    // используем только EndContact, так как уничтожение блоков будет производиться уже после отлёта шарика
    class blocksKickListener : public b2ContactListener {
    public:
        void EndContact(b2Contact* contact);
    };

    blocksKickListener listener;


    class physic_body {
    protected:
        b2World& world;

    private:
        object_type id;

    public:
        physic_body(b2World& world, object_type id);
        virtual ~physic_body();

        object_type getId() const;
    };

    class physic_ball : public physic_body {
    private:
        b2Body* ball;
        bool is_launched;
        object_type owner;

        b2Body* createBall(const float play_pos_x, const float play_pos_y, const bool is_top);

    public:
        physic_ball(b2World& world, const float play_pos_x, const float play_pos_y);
        ~physic_ball();

        void restart(const b2Vec2& player_position, const bool is_top);
        void lauch(const num_action push, const players pl);
        void move_with_player(const b2Vec2& speed, const num_move dest);

        void setOwner(object_type type);
        object_type getOwner() const;
        const b2Vec2& giveCoords() const;
        const b2Vec2 giveSpeed() const;
        bool isLaunched() const;
    };

    class physic_player : public physic_body {
    private:
        // указатели юзаем из-за особенностей либы (методы World принимают в качестве аргументов указатели и ничего больше)
        b2Body* player;
        b2Vec2 player_speed;
        bool isKicked;

    public:
        physic_player(b2World& world, const float start_x, const float start_y, const object_type id);
        ~physic_player();

        void stop();
        void move(const num_move dest);
        bool checkKicked();

        const b2Vec2 getSpeed() const;
        void setKicked();
        const b2Vec2& giveCoords() const;
    };

    class physic_border : public physic_body {
    private:
        b2Body* border;

    public:
        physic_border(b2World& world, const float pos_x, const float pos_y, const float size_x, const float size_y);
        ~physic_border();
    };

    class physic_block : public physic_body {
    private:
        b2Body* block;
        bool is_kicked;
        int number;

    public:
        physic_block(
                b2World& world,
                const float pos_x, const float pos_y,
                const float size_x, const float size_y,
                const float angle, const int number
        );

        ~physic_block();

        // удаление блока, если он был задет;
        // если блок не был ударен, возвращает -1 (неудача)
        // если был, возвращает номер блока в массиве
        int try_kick();

        void setKicked();
    };


    /* ------ Подвижные объекты ------ */
    physic_ball ball;
    physic_player player_bottom;
    physic_player player_top;


    /* ----- Статические объекты ----- */
    physic_border right_border;
    physic_border left_border;
    // делаем двух игроков => отключаем верхний барьер
    // physic_border top_border;

    /* ------------ Блоки ------------ */
    std::vector<physic_block*> blocks;

    /* ------ Инфа для логики -------- */
    int broken_block;
    bool sound_player;
    object_type who_broke_the_block;


    // настройки b2d
    const int32 velocityIterations = 8;
    const int32 positionIterations = 3;

    // SFML-ский таймер
    sf::Clock clock;
    float dt;


public:
    physics_scene(const float window_size_x, const float window_size_y);

    void analyseKeys(physic_player& player, const players who_leads_the_ball, const num_move move, const num_action action);
    void moveBall(physic_player& player, const num_move move);

    void calculate(
            const num_move key_bottom_move, const num_action key_bottom_action,
            const num_move key_top_move, const num_action key_top_action,
            const players who_lost_the_ball, const players who_leads_the_ball
    );

    const sf::Vector2f givePlayerBottomCoords() const;
    const sf::Vector2f givePlayerTopCoords() const;
    const sf::Vector2f giveBallCoords() const;
    const sf::Vector2f giveBallSpeed() const;

    const int getBrokenBlock() const;
    players getHitman() const;
    bool checkPlayerKicked();

    ~physics_scene();
};

#endif //PHYSICS_H
