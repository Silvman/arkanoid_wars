#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <SFML/Network.hpp>
#include <iostream>

#define PTM 30.0f



sf::Packet& operator << (sf::Packet& packet, const sf::Vector2f from) {
    return packet << from.x << from.y;
}

sf::Packet& operator >> (sf::Packet& packet, sf::Vector2f to) {
    return packet >> to.x >> to.y;
}


/*
 * TODO : в целом
 *
 * (+/-) Убрать хардкод! Пусть все константы определяются в logic_scene, а остальное от них зависит
 * (+, но есть костыли) Сделать по ООП-шному в каждом классе; разнести по player, ball и т.д. (как в logic_scene)
 *
 * (+) Перенести обработку клавиатуры из физики в logic
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


/* ------ Клиенская часть ------ */

// графическая сцена

class graphics_scene {
private:
    sf::RenderWindow& window;

    sf::RectangleShape player;
    sf::CircleShape ball;
    // block_body* blocks;

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


public:
    graphics_scene(sf::RenderWindow& window) : window(window) {
        player.setPosition(window.getSize().x / 2, window.getSize().y - 20);
        player.setSize(sf::Vector2f(100, 10));
        player.setOrigin(50, 5); // хардкод

        ball.setPosition(window.getSize().x / 2, window.getSize().y - 30);
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

class client {
private:
    char key;

    graphics_scene graphics;

    sf::TcpSocket socket;//программный интерфейс для обеспечения обмена данными между процессами
    sf::Packet packet;	//Для осуществления пакетной передачи дынных

    sf::Vector2f player_position;
    sf::Vector2f ball_position;

    /* еще инфа о блоках будет */

public:
    client(sf::RenderWindow& window, sf::IpAddress ip) : graphics(window), key(' ') {
        socket.connect(ip, 2000);
    }

    void run() {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            key = 'l';

        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            key = ' ';

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            key = 'r';

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            key = 'u';

        packet << key;
        socket.send(packet);
        packet.clear();

        socket.receive(packet);
        /*
         * Структура packet: sf::Vector2f player_position, sf::Vector2f ball_position
         */
        if(packet >> player_position >> ball_position) {
            graphics.draw(player_position, ball_position);
        }

        sleep(sf::milliseconds(10));
    }

    ~client() { socket.disconnect(); }
};

/* ----------------------------- */



/* ------ Серверная часть ------ */

// физика
/*
 * TODO : физика
 *
 * Сделать так чтобы платформа упиралась в стенку, а не прходила через нее
 * Физика столкновения - чтобы кидал флаги об ударенных блоках
 *
 */
class physics_scene {
private:
    b2World world;

    class physic_ball {
        private:
            b2World& world;
            b2Body* ball;
            b2Vec2 ball_speed;
            b2Joint* joint;
    
        public:
            physic_ball(b2World& world, const float play_pos_x, const float play_pos_y, const b2Vec2& speed) : world(world), ball_speed(speed) {
                b2BodyDef ball_def;
                ball_def.type = b2_dynamicBody;
                ball_def.position.Set(play_pos_x / PTM, (play_pos_y - 25) / PTM);
                ball = world.CreateBody(&ball_def);
    
                b2CircleShape ball_shape;
                ball_shape.m_radius = 10/PTM;
    
                b2FixtureDef ball_fixture_def;
                ball_fixture_def.shape = &ball_shape;
                ball_fixture_def.density = 10.1f;
                ball_fixture_def.restitution = 1.05f;
                ball_fixture_def.friction = 0.0f;
    
                ball->CreateFixture(&ball_fixture_def);
            }

            physic_ball& operator = (const physic_ball& from) {
                ball = from.ball;
                // this->ball_speed = ball_speed; // не нужно пока, т.к. под скоростью подразумевается начальная скорость
            }
    
            void restart(const b2Vec2& player_position) {
                world.DestroyBody(ball);
                //копипаст! D:
                b2BodyDef ball_def;
                ball_def.type = b2_dynamicBody;
                ball_def.position.Set(player_position.x, player_position.y - 25 / PTM); // тут еще остается ptm TODO
                ball = world.CreateBody(&ball_def);

                b2CircleShape ball_shape;
                ball_shape.m_radius = 10/PTM;

                b2FixtureDef ball_fixture_def;
                ball_fixture_def.shape = &ball_shape;
                ball_fixture_def.density = 10.1f;
                ball_fixture_def.restitution = 1.05f;
                ball_fixture_def.friction = 0.0f;

                ball->CreateFixture(&ball_fixture_def);
            }
            
            void lauch() {
                // std::cout << ball << std::endl;
                ball->SetLinearVelocity(ball_speed);
            }

            void move_with_player(const b2Vec2& speed, char dest) {
                switch (dest) {
                    case 'r': {
                        ball->SetLinearVelocity(speed);
                        break;
                    }

                    case 'l': {
                        ball->SetLinearVelocity(-speed);
                        break;
                    }

                    default: {
                        ball->SetLinearVelocity(b2Vec2(0,0));
                        break;
                    }
                }
            }

            const b2Vec2 giveCoords() const {
                return ball->GetPosition();
            }
            
            ~physic_ball() { world.DestroyBody(ball); }
    };
    
    class physic_player {
        private:
            b2World& world;
            // указатели юзаем из-за особенностей либы (методы World принимают в качестве аргументов указатели и ничего больше)
            b2Body* player;
            b2Vec2 player_speed;

        public:
            physic_player(b2World& world, const float start_x, const float start_y, const b2Vec2& speed) : world(world), player_speed(speed) {
                b2BodyDef player_def;
                player_def.type = b2_kinematicBody;
                player_def.position.Set(start_x / PTM, start_y / PTM);
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

            void stop() {
                player->SetLinearVelocity(b2Vec2(0,0));
            }

            void move(char dest) {
                switch (dest) {
                    case 'r': {
                        player->SetLinearVelocity(player_speed);
                        break;
                    }

                    case 'l': {
                        player->SetLinearVelocity(-player_speed);
                        break;
                    }

                    default: {
                        stop();
                        break;
                    }
                }
            }

            const b2Vec2& getSpeed() const {
                return player_speed;
            }

            const b2Vec2 giveCoords() const {
                return player->GetPosition();
            }

            ~physic_player() { world.DestroyBody(player); }
    };

    class physic_border {
        private:
            b2World& world;
            b2Body* border;

        public:

            physic_border(b2World& world, const float pos_x, const float pos_y, const float size_x, const float size_y) : world(world) {
                b2BodyDef border_def;
                b2PolygonShape border_shape;

                border_def.position.Set(pos_x/PTM, pos_y/PTM);
                border_shape.SetAsBox(size_x/PTM, size_y/PTM);
                border = world.CreateBody(&border_def);
                border->CreateFixture(&border_shape, 0.0f);
            }

            ~physic_border() { world.DestroyBody(border); }

    };

    physic_ball ball;

    physic_player player;

    physic_border right_border;

    physic_border left_border;

    physic_border top_border;

    bool is_launched;

    // b2Body** blocks;

    int32 velocityIterations = 8;
    int32 positionIterations = 3;


    sf::Clock clock;
    float dt;


public:
    physics_scene(const sf::RenderWindow& window)
            : world(b2Vec2(0.0f, 0.0f)),
              ball(world, (window.getSize().x / 2), (window.getSize().y - 35), b2Vec2(9.0f, -9.0f)),
              player(world, (window.getSize().x / 2), (window.getSize().y - 35), b2Vec2(15, 0.0f)),
              right_border(world, (window.getSize().x - 10), (window.getSize().y / 2), 10.0f, window.getSize().y / 2),
              left_border(world, 0.0f, (window.getSize().y / 2), 10.0f, window.getSize().y / 2),
              top_border(world, (window.getSize().x / 2), 0.0f, (window.getSize().x / 2), 10.0f),
              is_launched(false) { }

    void calculate(char key, bool ball_lost) {
        // TODO : при переносе на сервер - заменить sf::keyboard на данные от клиента
        dt = clock.getElapsedTime().asSeconds();
        clock.restart();

        // socket.receive(packet);

        if(ball_lost) {
            is_launched = false;
            ball.restart(player.giveCoords());
        }

        switch (key) {
            case 'l':
            case 'r': {
                player.move(key);
                if(!is_launched)
                    ball.move_with_player(player.getSpeed(), key);
                break;
            }

            case 'u': {
                if(!is_launched) {
                    is_launched = true;
                    ball.lauch();
                }
            }

            default: {
                player.stop();
                if(!is_launched)
                    ball.move_with_player(player.getSpeed(), ' ');
            }
        }

        world.Step(dt, velocityIterations, positionIterations);
        std::cout << dt << std::endl;
    }

    const sf::Vector2f givePlayerCoords() const {
        b2Vec2 coords = player.giveCoords();
        return sf::Vector2f(coords.x * PTM, coords.y * PTM);
    }

    const sf::Vector2f giveBallCoords() const {
        b2Vec2 coords = ball.giveCoords();
        return sf::Vector2f(coords.x * PTM, coords.y * PTM);
    }

    ~physics_scene() { }
};

// логика игры
class logic_world {
private:

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

    logic_player player;
    logic_ball ball;

    bool ball_lost;

    physics_scene physics;

    const float bottom_border;


public:
    logic_world(sf::RenderWindow& window)
            : player(window.getSize().x / 2.0f),
              ball(window.getSize().x / 2.0f, window.getSize().y - 35.0f),
              physics(window),
              ball_lost(false),
              bottom_border(window.getSize().y) { }

    void update(char key) {

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

        physics.calculate(key, ball_lost);
        ball_lost = false;


        player.setPosition(physics.givePlayerCoords());
        ball.setPosition(physics.giveBallCoords());

        //graphics.draw(player.getPosition(), ball.getPosition());
        // вместо этой фигни - отправка новых координат

        sleep(sf::milliseconds(10));
    }

    ~logic_world() { }
};

class server {
private:

public:

};

/* ----------------------------- */



int main() {
    // Создаем главное окно приложения
    sf::RenderWindow window(sf::VideoMode(1024, 800), "Arcanoid!");
    window.setFramerateLimit(50);
    window.setVerticalSyncEnabled(true);

    // logic_world game_o(window);

    client game(window, sf::IpAddress("192.168.0.1"));

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

        game.run();

        // game.update();
    }

    return 0;
}