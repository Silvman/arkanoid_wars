#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <iostream>

#define PTM 30.0f

/*
 * TODO : в целом
 *
 * Убрать хардкод! Пусть все константы определяются в logic_scene, а остальное от них зависит
 * Сделать по ООП-шному в каждом классе; разнести по player, ball и т.д. (как в logic_scene)
 *
 * Сделать массив блоков
 * Сделать удара шарика о блок
 * Сделать игровую логику - разбил все блоки и lives >= 0 - победил, lives < 0 - проиграл
 *
 * Сделать начальный экран
 * Сделать экраны, вылетающие при поражении\победе
 *
 * Прикрутить второго игрока
 * Прикрутить сервер
 *
 * Сделать графоний и звук
 *
 */

// графическая сцена

// пока незаюзано, но будет :)
class block_body {
private:
    sf::RectangleShape body;
public:
    block_body(const float x_start, const float y_start, const float height, const float width, const float angle = 0) {
        sf::Vector2f size(height, width);

        body.setPosition(x_start, y_start);
        body.setSize(size);
        body.setOrigin(height/2, width/2); // хардкод?
        body.setRotation(angle);
    }
    // как удалять блоки
    ~block_body(){};

};

class graphics_scene {
private:
    sf::RenderWindow& window;

    sf::RectangleShape player;
    sf::CircleShape ball;
    // block_body* blocks;

public:
    graphics_scene(sf::RenderWindow& window) : window(window) {
        player.setPosition(400, 580);
        player.setSize(sf::Vector2f(100, 10));
        player.setOrigin(50, 5); // хардкод

        ball.setPosition(400, 570);
        ball.setRadius(10);
    }

    void draw(const sf::Vector2f player_coords, const sf::Vector2f ball_coords) {
        window.clear();

        player.setPosition(player_coords);
        ball.setPosition(ball_coords);

        window.draw(player);
        window.draw(ball);

        // Отрисовка
        window.display();
    }

    ~graphics_scene() {}
};


/*
 * TODO : физика
 *
 * Сделать так, чтобы платформа двигалась только вдоль x (сейчас может дергаться)
 * Сделать так, чтобы скорость шарика не падала
 *
 */

// физика
class physics_scene {
private:
    b2World world;

    b2Body* ball;
    b2Vec2 ball_speed;
    bool is_launched;

    b2Body* player;
    b2Vec2 player_speed;


    b2Body* borders[3];
    // b2Body** blocks;

    int32 velocityIterations = 8;
    int32 positionIterations = 3;


public:
    physics_scene()
            : world(b2Vec2(0.0f, 0.0f)),
              ball_speed(b2Vec2(10.0f, -10.0f)),
              player_speed(b2Vec2(9, 0.0f)),
              is_launched(false) {

        // creating borders

        b2BodyDef border_def;
        b2PolygonShape border_shape;

        border_def.position.Set(790.0f/PTM, 300.0f/PTM); // resolution
        border_shape.SetAsBox(10.0f/PTM, 300.0f/PTM);
        borders[0] = world.CreateBody(&border_def);
        borders[0]->CreateFixture(&border_shape, 0.0f);

        border_def.position.Set(0.0f/PTM, 300.0f/PTM); // resolution
        borders[1] = world.CreateBody(&border_def);
        borders[1]->CreateFixture(&border_shape, 0.0f);

        border_def.position.Set(400.0f/PTM, 0/PTM);
        border_shape.SetAsBox(400.0f/PTM, 10.0f/PTM);
        borders[2] = world.CreateBody(&border_def);
        borders[2]->CreateFixture(&border_shape, 0.0f);


        // creating ball

        b2BodyDef ball_def;
        ball_def.type = b2_dynamicBody;
        ball_def.position.Set(400.0f/PTM, 555.0f/PTM);
        ball = world.CreateBody(&ball_def);

        b2CircleShape ball_shape;
        ball_shape.m_radius = 10/PTM;

        b2FixtureDef ball_fixture_def;
        ball_fixture_def.shape = &ball_shape;
        ball_fixture_def.density = 10.1f;
        ball_fixture_def.restitution = 2;
        ball_fixture_def.friction = 0.0f;

        ball->CreateFixture(&ball_fixture_def);


        // creating player

        b2BodyDef player_def;
        player_def.type = b2_dynamicBody;
        player_def.position.Set(400.0f/PTM, 580.0f/PTM);
        player = world.CreateBody(&player_def);

        b2PolygonShape player_shape;
        player_shape.SetAsBox(50.0f/PTM, 5.0f/PTM);

        b2FixtureDef player_fixture_def;
        player_fixture_def.shape = &player_shape;
        player_fixture_def.density = 10.1f;
        player_fixture_def.restitution = 1;
        player_fixture_def.friction = 0.0f;

        player->CreateFixture(&player_fixture_def);

    }

    void calculate(float dt, bool ball_lost) {
        // TODO : при переносе на сервер - заменить sf::keyboard на данные от клиента

        if(ball_lost) {
            is_launched = false;
            world.DestroyBody(ball);

            b2BodyDef ball_def;
            ball_def.type = b2_dynamicBody;
            ball_def.position.Set(player->GetPosition().x, player->GetPosition().y - 25/PTM);
            ball = world.CreateBody(&ball_def);

            b2CircleShape ball_shape;
            ball_shape.m_radius = 10/PTM;

            b2FixtureDef ball_fixture_def;
            ball_fixture_def.shape = &ball_shape;
            ball_fixture_def.density = 10.1f;
            ball_fixture_def.restitution = 2;
            ball_fixture_def.friction = 0.0f;

            ball->CreateFixture(&ball_fixture_def);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->SetLinearVelocity(-player_speed);
            if(!is_launched) {
                ball->SetLinearVelocity(-player_speed);
            }
        }

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player->SetLinearVelocity(b2Vec2(0, 0));
            if(!is_launched) {
                ball->SetLinearVelocity(b2Vec2(0, 0));
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player->SetLinearVelocity(player_speed);
            if(!is_launched) {
                ball->SetLinearVelocity(player_speed);
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !is_launched) {
            is_launched = true;
            ball->SetLinearVelocity(ball_speed);
        }

        world.Step(dt, velocityIterations, positionIterations);
        std::cout << dt << " " << ball->GetLinearVelocity().x << std::endl;
    }

    const sf::Vector2f givePlayerCoords() const {
        b2Vec2 b2_coords = player->GetPosition();
        return sf::Vector2f(b2_coords.x * PTM, b2_coords.y * PTM);
    }

    const sf::Vector2f giveBallCoords() const {
        b2Vec2 b2_coords = ball->GetPosition();
        return sf::Vector2f(b2_coords.x * PTM, b2_coords.y * PTM);
    }

    ~physics_scene() { }
};


// класс Игрок (для основной сцены)
class logic_player {
private:
    sf::Vector2f position;
    int score, lives;

public:
    logic_player(const float x_start = 580, const float y_start = 400) : position(x_start, y_start), score(0), lives(3) { }

    void setPosition(sf::Vector2f new_position) {
        position = new_position;
    }

    const sf::Vector2f getPosition() const {
        return position;
    }

    void setScore(const int new_score) {
        score = new_score;
    }

    int getScore() const {
        return score;
    }

    void setLives(const int new_lives) {
        lives = new_lives;
    }

    int getLives() const {
        return lives;
    }

    void operator ++ () {
        score++;
    }

    ~logic_player() {}
};

class logic_ball {
private:
    sf::Vector2f position;

public:

    logic_ball(const float start_x = 570, const float start_y = 400) : position(start_x, start_y) { }

    void setPosition(sf::Vector2f new_position) {
        position = new_position;
    }

    const sf::Vector2f getPosition() const {
        return position;
    }

    ~logic_ball() { }
};

class logic_world {
private:
    logic_player player;
    logic_ball ball;

    bool ball_lost;

    physics_scene physics;
    graphics_scene graphics;

    sf::Clock clock;
    float time;

    const float bottom_border = 600;


public:
    logic_world(sf::RenderWindow& window, const float x_start, const float y_start)
            : player(x_start),
              ball(x_start, y_start + 10),
              graphics(window),
              ball_lost(false) { }

    void update() {
        time = clock.getElapsedTime().asSeconds();
        clock.restart();

        // шарик ушел за пределы поля
        if(ball.getPosition().y > bottom_border) {
            player.setLives(player.getLives() - 1);
            ball_lost = true;
        }

        // жизней меньше нуля
        if(player.getLives() < 0) {
            // ты проиграл!
        }

        /*
         * Тут должен располагаться цикл, в котором будем проверять, не сбили ли какой-то блок
         */

        physics.calculate(time, ball_lost);
        ball_lost = false;

        player.setPosition(physics.givePlayerCoords());
        ball.setPosition(physics.giveBallCoords());

        graphics.draw(player.getPosition(), ball.getPosition());
    }

    ~logic_world() { }
};


int main()
{
    // Создаем главное окно приложения
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arcanoid!");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    logic_world game (window, 580, 400);

    // Главный цикл приложения
    while(window.isOpen())
    {
        // Обрабатываем события в цикле
        sf::Event event;
        while(window.pollEvent(event))
        {
            // Кроме обычного способа наше окно будет закрываться по нажатию на Escape
            if(event.type == sf::Event::Closed ||
               (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                window.close();
        }

        game.update();
    }

    return 0;
}