#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <SFML/Network.hpp>
#include <iostream>

#define PTM 30.0f
#define DEG 57.29577f

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
 * (+) Прикрутить второго игрока
 * (+) Прикрутить сервер
 *
 * Сделать графоний и звук
 *
 */


/*
 * TODO: сервер и клиент
 *
 * Сделать подключение сокетов через selector
 * Клиент продолжает посылать данные даже после обрыва соединения. Это надо обрабатывать.
 *
 */


/* ------ Клиенская часть ------ */

// графическая сцена

class graphics_scene {
private:
    sf::RenderWindow& window;
	sf::Font font;

    sf::RectangleShape player_bottom;
    sf::RectangleShape player_top;
    sf::CircleShape ball;
	sf::Text conect;
    
    sf::RectangleShape block;
    
    /*
    class block_block {
    private:
        sf::RectangleShape block;
        bool isExists;

    public:
        block_block(const float x_start, const float y_start, const float height, const float width, const float angle = 0) {
            sf::Vector2f size(height, width);

            block.setPosition(x_start, y_start);
            block.setSize(size);
            block.setOrigin(height/2, width/2); // хардкод?
            block.setRotation(angle);
        }
        // как удалять блоки

        void draw(sf::RenderWindow& window) {
            if(isExists) {
                window.draw(block);
            }
        }

        void destroy() {
            isExists = false;
        }

        const sf::RectangleShape& getShape() const {
            return block;
        }

        ~block_block(){};

    };

    block_block test_block;
     */

public:
    graphics_scene(sf::RenderWindow& window) : window(window) {
        // TODO: тут координаты не сходятся с теми, что в logic

        font.loadFromFile("/home/silvman/CLionProjects/arkanoid_mult/CyrilicOld.TTF");

        player_bottom.setPosition(window.getSize().x / 2, window.getSize().y - 20);
        player_bottom.setSize(sf::Vector2f(100, 10));
        player_bottom.setOrigin(50, 5); // хардкод

		conect.setColor(sf::Color::Red);
		conect.setFont(font);
		conect.setCharacterSize(50);
		conect.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 );
        
        player_top.setPosition(window.getSize().x / 2, 35);
        player_top.setSize(sf::Vector2f(100, 10));
        player_top.setOrigin(50, 5); // хардкод

        ball.setPosition(window.getSize().x / 2, window.getSize().y - 30);
        ball.setRadius(10);

        block.setPosition(window.getSize().x / 2, window.getSize().y / 2);
        block.setSize(sf::Vector2f(40.0f, 40.0f));
        block.setOrigin(20.0f, 20.0f); // хардкод?
        block.setRotation(30);

    }

    void draw(const sf::Vector2f& player_bottom_coords, const sf::Vector2f& player_top_coords,
			  const sf::Vector2f& ball_coords, bool canEarseBlock, bool connection) {
        window.clear();

		if (connection == 0) {
			conect.setString("Problems with connecting \n another player");

			window.draw(conect);
		}

        player_bottom.setPosition(player_bottom_coords);
        player_top.setPosition(player_top_coords);
        ball.setPosition(ball_coords);



        window.draw(block);
        window.draw(player_bottom);
        window.draw(player_top);
        window.draw(ball);

        // Отрисовка
        window.display();
    }

	void draw_er_con() {
		window.clear();

		conect.setString("Problems with connection");
		window.draw(conect);

		window.display();
	}

	void draw_wait_con(){

			window.clear();
			conect.setString("Waiting other player");
			window.draw(conect);

			window.display();
	}

    ~graphics_scene() {}
};

class client {
private:
    unsigned my_number;
	bool connection;
    unsigned int key_move, key_action;

    graphics_scene graphics;

    sf::TcpSocket socket;//программный интерфейс для обеспечения обмена данными между процессами
    sf::Packet INPUT;	//Для осуществления пакетной передачи дынных
	sf::Packet OUTPUT;

    sf::Vector2f player_bottom_position;
    sf::Vector2f player_top_position;
    sf::Vector2f ball_position;

    bool canEarseBlock;

    /*
    float x_bottom_player, y_bottom_player;
    float x_top_player, y_top_player;
	float x_ball, y_ball;
    */

    /* еще инфа о блоках будет */

public:
    client(sf::RenderWindow& window, sf::IpAddress ip) : graphics(window), key_move(0), key_action(0) {
		socket.connect(ip, 2000);
        std::cout << "Connected with server" << std::endl;

        socket.receive(INPUT);

        INPUT >> my_number;
        std::cout << "my number is: " << my_number << std::endl;
		graphics.draw_wait_con();
    }

    void run(sf::RenderWindow& window) {
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

		std::cout << "key_move: " << key_move << ", key_action: " << key_action << std::endl;
		OUTPUT << my_number << key_move << key_action;
        socket.send(OUTPUT);
		OUTPUT.clear();

        if (socket.receive(INPUT) == sf::Socket::Done){
			std::cout << "Packet recieved"<< std::endl;
			INPUT >> connection;
			INPUT >> player_bottom_position.x >> player_bottom_position.y;
			INPUT >> player_top_position.x >> player_top_position.y;
        	INPUT >> ball_position.x >> ball_position.y;
            INPUT >> canEarseBlock;

        	std::cout << "player bottom: (" << player_bottom_position.x << ", " << player_bottom_position.y << ")" << std::endl;
        	std::cout << "player top: (" << player_top_position.x << ", " << player_top_position.y << ")" << std::endl;
        	std::cout << "ball: (" << ball_position.x << ", " << ball_position.y << ")" << std::endl;

			graphics.draw(player_bottom_position, player_top_position, ball_position, canEarseBlock, connection);

		} else graphics.draw_er_con();

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

    class blocksKickListener : public b2ContactListener {
    public:
        void BeginContact(b2Contact* contact) { }

        void EndContact(b2Contact* contact) {
            std::cout << "I'm working" << std::endl;

            physic_body* bodyUserData = static_cast<physic_body*>(contact->GetFixtureA()->GetBody()->GetUserData());

            std::cout << bodyUserData->getId() << std::endl;
            if ( bodyUserData->getId() == 4 ) {
                std::cout << "Fir!" << std::endl;
                reinterpret_cast<physic_block*>(bodyUserData)->setKicked();
            }

            bodyUserData = static_cast<physic_block*>(contact->GetFixtureB()->GetBody()->GetUserData());
            std::cout << bodyUserData->getId() << std::endl;
            if ( bodyUserData->getId() == 4 ) {
                std::cout << "Fir!" << std::endl;
                reinterpret_cast<physic_block*>(bodyUserData)->setKicked();
            }
        }
    };

    blocksKickListener listener;

    class physic_body {
    private:
        int id;

    public:
        physic_body(int id) : id(id) {}

        int getId() const {
            return id;
        }

        virtual ~physic_body() {}
    };

    class physic_ball: public physic_body {
        private:
            b2World& world;
            b2Body* ball;
            b2Vec2 ball_speed;

            b2Body* createBall(const float play_pos_x, const float play_pos_y, const bool is_top) {
                b2Body* new_ball;

                b2BodyDef ball_def;
                ball_def.type = b2_dynamicBody;

                if(is_top) {
                    ball_def.position.Set(play_pos_x, play_pos_y + 5 / PTM); // тут еще остается ptm TODO
                    // TODO: сделать нормальное изменение скорости
                    ball_speed = b2Vec2(ball_speed.x, fabsf(ball_speed.y));
                } else {
                    ball_def.position.Set(play_pos_x, play_pos_y - 15 / PTM); // тут еще остается ptm
                    ball_speed = b2Vec2(ball_speed.x, -fabsf(ball_speed.y));
                }

                new_ball = world.CreateBody(&ball_def);

                b2CircleShape ball_shape;
                ball_shape.m_radius = 10/PTM;

                b2FixtureDef ball_fixture_def;
                ball_fixture_def.shape = &ball_shape;
                ball_fixture_def.density = 10.1f;
                ball_fixture_def.restitution = 1.05f;
                ball_fixture_def.friction = 0.0f;

                // из-за особенностей хранения userData
                new_ball->CreateFixture(&ball_fixture_def);

                new_ball->SetUserData(this);

                return new_ball;
            };

        public:
            physic_ball(b2World& world, const float play_pos_x, const float play_pos_y, const b2Vec2& speed)
                    : physic_body(3), world(world), ball_speed(speed) {
                // false - так как игра всегда начинается с нижнего игрока
                ball = createBall(play_pos_x, play_pos_y, false);
            }

            void restart(const b2Vec2& player_position, const bool is_top) {
                world.DestroyBody(ball);

                ball = createBall(player_position.x, player_position.y, is_top);
            }

            void lauch() {
                // std::cout << ball << std::endl;
                ball->SetLinearVelocity(ball_speed);
            }

            void move_with_player(const b2Vec2& speed, const unsigned int dest) {
                switch (dest) {
                    case 2: {
                        ball->SetLinearVelocity(speed);
                        break;
                    }

                    case 1: {
                        ball->SetLinearVelocity(-speed);
                        break;
                    }

                    // дописал default, ибо не по православному без него
                    case 0:
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

    class physic_player: public physic_body {
        private:
            b2World& world;
            // указатели юзаем из-за особенностей либы (методы World принимают в качестве аргументов указатели и ничего больше)
            b2Body* player;
            b2Vec2 player_speed;

        public:
            physic_player(b2World& world, const float start_x, const float start_y, const b2Vec2& speed, const int id)
                    : physic_body(id), world(world), player_speed(speed) {
                b2BodyDef player_def;
                player_def.type = b2_kinematicBody;
                player_def.position.Set((start_x - 10) / PTM, (start_y - 10) / PTM);
                player = world.CreateBody(&player_def);

                b2PolygonShape player_shape;
                player_shape.SetAsBox(50.0f/PTM, 5.0f/PTM);

                b2FixtureDef player_fixture_def;
                player_fixture_def.shape = &player_shape;
                player_fixture_def.density = 10.1f;
                player_fixture_def.restitution = 1;
                player_fixture_def.friction = 0.0f;

                player->CreateFixture(&player_fixture_def);
                player->SetUserData(this);
            }

            void stop() {
                player->SetLinearVelocity(b2Vec2(0,0));
            }

            void move(const unsigned int dest) {
                switch (dest) {
                    case 2: {
                        player->SetLinearVelocity(player_speed);
                        break;
                    }

                    case 1: {
                        player->SetLinearVelocity(-player_speed);
                        break;
                    }

                    default:
                    case 0: {
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

    class physic_border : public physic_body {
        private:
            b2World& world;
            b2Body* border;

        public:

            physic_border(b2World& world, const float pos_x, const float pos_y, const float size_x, const float size_y)
                    : physic_body(5), world(world) {
                b2BodyDef border_def;
                b2PolygonShape border_shape;

                border_def.position.Set((pos_x - 10)/PTM, (pos_y - 10)/PTM);
                border_shape.SetAsBox(size_x/PTM, size_y/PTM);
                border = world.CreateBody(&border_def);
                border->CreateFixture(&border_shape, 0.0f);

                border->SetUserData(this);
            }

            ~physic_border() { world.DestroyBody(border); }

    };

    class physic_block : public physic_body {
    private:
        b2World& world;
        b2Body* block;
        bool is_kicked;
        int number;

    public:
        physic_block(
                b2World& world,
                const float pos_x, const float pos_y,
                const float size_x, const float size_y,
                const float angle, const int number
        ) : physic_body(4), world(world), is_kicked(false), number(number) {
                b2BodyDef block_def;
                b2PolygonShape block_shape;

                block_def.position.Set((pos_x - 10)/PTM, (pos_y - 10)/PTM);
                block_def.angle = angle / DEG;
                block_shape.SetAsBox(size_x / PTM, size_y / PTM);
                block = world.CreateBody(&block_def);
                block->CreateFixture(&block_shape, 0.0f);

                block->SetUserData(this);
        }

        void try_kick() {
            if(is_kicked) {
                std::cout << "Tried :c" << std::endl;
                block->SetActive(0);
            }
        }

        void setKicked() {
            is_kicked = true;
        }

        ~physic_block() { }
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

    bool is_launched;

    int32 velocityIterations = 8;
    int32 positionIterations = 3;

    // SFML-ский таймер
    sf::Clock clock;
    float dt;


public:
    // каждому телу соответствует свой собственный id в зависимотси от рода
    // 1 - платформа нижняя, 2 - платформа верхняя, 3 - шарик, 4 - блок, 5 - стенка

    physics_scene(const float window_size_x, const float window_size_y)
            : world(b2Vec2(0.0f, 0.0f)),
              ball(world, (window_size_x/ 2), (window_size_y - 35), b2Vec2(0.0f, -9.0f)),
              player_bottom(world, (window_size_x / 2), (window_size_y - 35), b2Vec2(15, 0.0f), 1),
              player_top(world, (window_size_x / 2), 35, b2Vec2(15, 0.0f), 2),
              right_border(world, (window_size_x - 10), (window_size_y / 2), 10.0f, window_size_y / 2),
              left_border(world, 0.0f, (window_size_y / 2), 10.0f, window_size_y / 2),
              // top_border(world, (window_size_x / 2), 0.0f, (window_size_x / 2), 10.0f),
              // test_block(world, (window_size_x / 2), (window_size_y / 2), 20.0f, 20.0f, 30.0f, 1),
              is_launched(false)
    {
        world.SetContactListener(&listener);

        blocks.push_back(new physic_block(world, 70, 100, 60.0f, 20.0f, 0.0f, 1));
        blocks.push_back(new physic_block(world, 910, 100, 60.0f, 20.0f, 0.0f, 2));

        blocks.push_back(new physic_block(world, 190, 140, 60.0f, 20.0f, 0.0f, 3));
        blocks.push_back(new physic_block(world, 310, 140, 60.0f, 20.0f, 0.0f, 4));
        blocks.push_back(new physic_block(world, 430, 140, 60.0f, 20.0f, 0.0f, 5));
        blocks.push_back(new physic_block(world, 550, 140, 60.0f, 20.0f, 0.0f, 6));
        blocks.push_back(new physic_block(world, 670, 140, 60.0f, 20.0f, 0.0f, 7));
        blocks.push_back(new physic_block(world, 790, 140, 60.0f, 20.0f, 0.0f, 8));

        blocks.push_back(new physic_block(world, 190, 180, 60.0f, 20.0f, 0.0f, 9));
        blocks.push_back(new physic_block(world, 430, 180, 60.0f, 20.0f, 0.0f, 10));
        blocks.push_back(new physic_block(world, 790, 180, 60.0f, 20.0f, 0.0f, 11));

        blocks.push_back(new physic_block(world, 190, 220, 60.0f, 20.0f, 0.0f, 12));
        blocks.push_back(new physic_block(world, 550, 220, 60.0f, 20.0f, 0.0f, 13));
        blocks.push_back(new physic_block(world, 790, 220, 60.0f, 20.0f, 0.0f, 14));

        blocks.push_back(new physic_block(world, 190, 260, 60.0f, 20.0f, 0.0f, 15));
        blocks.push_back(new physic_block(world, 310, 260, 60.0f, 20.0f, 0.0f, 16));
        blocks.push_back(new physic_block(world, 430, 260, 60.0f, 20.0f, 0.0f, 17));
        blocks.push_back(new physic_block(world, 550, 260, 60.0f, 20.0f, 0.0f, 18));
        blocks.push_back(new physic_block(world, 670, 260, 60.0f, 20.0f, 0.0f, 19));
        blocks.push_back(new physic_block(world, 790, 260, 60.0f, 20.0f, 0.0f, 20));

        blocks.push_back(new physic_block(world, 70, 300, 60.0f, 20.0f, 0.0f, 21));
        blocks.push_back(new physic_block(world, 910, 300, 60.0f, 20.0f, 0.0f, 22));
    }

    void analyseKeys(physic_player& player, const unsigned move, const unsigned action) {
        switch (move) {
            case 1:
            case 2: {
                player.move(move);
                break;
            }

            default:
            case 0: {
                player.stop();
                break;
            }
        }

        switch (action) {
            case 1: {
                if(!is_launched) {
                    is_launched = true;
                    ball.lauch();
                }
                break;
            }

            default: break;
        }
    }


   // костыльная тема
    void moveBall(physic_player& player, const unsigned move) {
        switch (move) {
            case 1:
            case 2: {
                if(!is_launched)
                    ball.move_with_player(player.getSpeed(), move);
                break;
            }

            default:
            case 0: {
                if(!is_launched)
                    ball.move_with_player(player.getSpeed(), 0);
                break;
            }
        }
    }

    void calculate(
            const unsigned int key_bottom_move, const unsigned int key_bottom_action,
            const unsigned int key_top_move, const unsigned int key_top_action,
            const int who_lost_the_ball, const int who_leads_the_ball) {

        dt = clock.getElapsedTime().asSeconds();
        clock.restart();


        switch (who_lost_the_ball) {
            case 1: {
                is_launched = false;
                // первый параметр передает координаты игрока, потерявшего шарик
                // второй параметр - является ли положение верхним
                ball.restart(player_bottom.giveCoords(), false);
                break;
            }

            case 2: {
                is_launched = false;
                ball.restart(player_top.giveCoords(), true);
                break;
            }

            default: break;
        }

        analyseKeys(player_bottom, key_bottom_move, key_bottom_action);
        analyseKeys(player_top, key_top_move, key_top_action);

        if (who_leads_the_ball == 1) {
            moveBall(player_bottom, key_bottom_move);
        } else {
            moveBall(player_top, key_top_move);
        }

        for(auto it = blocks.begin(); it != blocks.end(); it++) {
            (*it)->try_kick();
        }

        world.Step(dt, velocityIterations, positionIterations);


        //std::cout << dt << std::endl;
    }

    const sf::Vector2f givePlayerBottomCoords() const {
        b2Vec2 coords = player_bottom.giveCoords();
        return sf::Vector2f(coords.x * PTM, coords.y * PTM);
    }

    const sf::Vector2f givePlayerTopCoords() const {
        b2Vec2 coords = player_top.giveCoords();
        return sf::Vector2f(coords.x * PTM, coords.y * PTM);
    }

    const sf::Vector2f giveBallCoords() const {
        b2Vec2 coords = ball.giveCoords();
        return sf::Vector2f(coords.x * PTM, coords.y * PTM);
    }

    ~physics_scene() {
        for(auto it = blocks.begin(); it != blocks.end(); it++) {
            delete(*it);
        }
    }
};

// логика игры
class logic_world {
private:

    class logic_player {
    private:
        sf::Vector2f position;
        int score, lives;

    public:
        logic_player(const float x_start, const float y_start) : position(x_start, y_start), score(0), lives(3) { }

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

        logic_ball(const float start_x, const float start_y) : position(start_x, start_y) { }

        void setPosition(sf::Vector2f new_position) {
            position = new_position;
        }

        const sf::Vector2f getPosition() const {
            return position;
        }

        ~logic_ball() { }
    };

    // (1) первый игрок - снизу
    logic_player player_bottom;

    // (2) второй игрок - сверху
    logic_player player_top;

    // мячик
    logic_ball ball;

    /*
     * Место под логические блоки
     */

    // Флаг, содержащий информацию о том, кто потерял шарик, и (== 0) если шарик не потерян
    int who_lost_the_ball;
    int who_leads_the_ball;

    physics_scene physics;

    bool isBlockExist;

    const float bottom_border;


public:
    logic_world(const float window_size_x,const float window_size_y)
            : player_bottom(window_size_x / 2.0f, window_size_y - 25.0f),
              player_top(window_size_x / 2.0f, window_size_y - 25.0f),
              ball(window_size_x / 2.0f, window_size_y - 35.0f),
              physics(window_size_x, window_size_y),
              who_lost_the_ball(0),
              who_leads_the_ball(1),
              isBlockExist(1),
              bottom_border(window_size_y) { }

    void update(
            const unsigned int key_bottom_move, unsigned int key_bottom_action,
            const unsigned int key_top_move, unsigned int key_top_action
    ) {

        // шарик ушел за пределы поля
        if (ball.getPosition().y > bottom_border) {
            player_bottom.setLives(player_bottom.getLives() - 1);
            who_lost_the_ball = 1;
        }

        if (ball.getPosition().y < 0) {
            player_top.setLives(player_top.getLives() - 1);
            who_lost_the_ball = 2;
        }

        // жизней меньше нуля
        if (player_bottom.getLives() < 0) {
            // первый проиграл
        }


        if (player_top.getLives() < 0) {
            // второй проиграл
        }

        /*
         * Тут должен располагаться цикл, в котором будем проверять, не сбили ли какой-то блок
         */

        // обрабатываем кажатые клавишы с учетом того, у кого шарик - у верхнего или у нижнего
        if (who_leads_the_ball == 1) {
            // если шарик у нижнего, верхний не может его запустить
            if(key_top_action == 1)
                key_top_action = 0;

        } else {
            if(key_bottom_action == 1)
                key_bottom_action = 0;

        }

        physics.calculate(key_bottom_move, key_bottom_action,
                          key_top_move, key_top_action,
                          who_lost_the_ball, who_leads_the_ball);


        isBlockExist = false;

        // шарик отдается тому, кто его упустил
        if(who_lost_the_ball != 0)
            who_leads_the_ball = who_lost_the_ball;

        // обработали потерю шарика, можем вернуть переменной исходное значение
        who_lost_the_ball = 0;

        player_bottom.setPosition(physics.givePlayerBottomCoords());
        player_top.setPosition(physics.givePlayerTopCoords());
        ball.setPosition(physics.giveBallCoords());

        sleep(sf::milliseconds(10));
    }

	const sf::Vector2f givePlayerTopCoords() const {
        // WARNING: меняю источник - теперь тянет не из physics, а из этого класса
		return player_top.getPosition();
	}

    const sf::Vector2f givePlayerBottomCoords() const {
        // WARNING: меняю источник - теперь тянет не из physics, а из этого класса
        return player_bottom.getPosition();
    }

	const sf::Vector2f giveBallCoords() const {
		return ball.getPosition();
	}

    const bool canEarseBlock() {
        return !isBlockExist;
    };

    ~logic_world() { }
};

struct DATA{
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

   /* unsigned int key_bottom_move;
    unsigned int key_bottom_action;
    unsigned int key_top_move;
    unsigned int key_top_action;

    unsigned num;
*/

public:
	server(const unsigned short port) : serv_world(1024,768) {
		listener.listen(port);
		std::cout << "Server is listening to port " << port << ", waiting for connection 1. " << std::endl;

        listener.accept(socket_1);
        OUTPUT << 1;
        socket_1.send(OUTPUT);
        OUTPUT.clear();

        std::cout << "Client 1 is connected, ip " << socket_1.getRemoteAddress() << std::endl;

        std::cout << "Server is listening to port " << port << ", waiting for connection 2. " << std::endl;
        listener.accept(socket_2);
        OUTPUT << 2;
        socket_2.send(OUTPUT);
        OUTPUT.clear();

        std::cout << "Client 2 is connected, ip " << socket_2.getRemoteAddress() << std::endl;
    }

	void input() {
        // Серв должен принимать 4 значения-ключа - движение нижнего, действие нижнего, движение верхнего, действие верхнего
        // В движениях: 1 - влево, 2 - вправо, 0 - по-умолчанию
        // В действиях  1 - запуск шара

		if (socket_1.receive(INPUT) == sf::Socket::Done ) {
			data.bot_connect = 1;

			INPUT >> data.num >> data.key_bottom_move >> data.key_bottom_action;
			//std::cout << "Socket 1. Recieved: " << data.num << ", " << data.key_bottom_move <<
			//		", " << data.key_bottom_action << std::endl;

			INPUT.clear();

		} else data.bot_connect = 0;

		if(socket_2.receive(INPUT) == sf::Socket::Done) {
			data.top_connect = 1;

			INPUT >> data.num >> data.key_top_move >> data.key_top_action;
			//std::cout << "Socket 2. Recieved: " << data.num << ", " << data.key_top_move <<
			//		", " << data.key_top_action << std::endl;

			INPUT.clear();
		} else data.top_connect = 0;
	}


	void output() {
		if (data.bot_connect == 1 && data.top_connect == 1) {
			serv_world.update(data.key_bottom_move, data.key_bottom_action, data.key_top_move, data.key_top_action);

			connection = 1;

			OUTPUT << connection;
			OUTPUT << serv_world.givePlayerBottomCoords().x << serv_world.givePlayerBottomCoords().y;
			OUTPUT << serv_world.givePlayerTopCoords().x << serv_world.givePlayerTopCoords().y;
			OUTPUT << serv_world.giveBallCoords().x << serv_world.giveBallCoords().y;
            OUTPUT << serv_world.canEarseBlock();

			socket_1.send(OUTPUT);
			socket_2.send(OUTPUT);
			OUTPUT.clear();

		} else {
			OUTPUT << connection;
			OUTPUT << serv_world.givePlayerBottomCoords().x << serv_world.givePlayerBottomCoords().y;
			OUTPUT << serv_world.givePlayerTopCoords().x << serv_world.givePlayerTopCoords().y;
			OUTPUT << serv_world.giveBallCoords().x << serv_world.giveBallCoords().y;
            OUTPUT << serv_world.canEarseBlock();
		}

		if(data.bot_connect == 1 && data.top_connect == 0) {
			socket_1.send(OUTPUT);
			OUTPUT.clear();
		}

		if(data.bot_connect == 0 && data.top_connect == 1) {
			socket_2.send(OUTPUT);
			OUTPUT.clear();
		}

		connection = 0;

	}

	~server() {
		socket_1.disconnect();
        socket_2.disconnect();
		listener.close();
	};

};

/* ----------------------------- */



int main() {

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();	//Локальный ip Адресс
	std::cout << ip << std::endl;
	char mode;
	std::cin >> mode;
	// logic_world game_o(window);
	if (mode == 'c') {
        std::cin >> ip;
		// Создаем главное окно приложения
		sf::RenderWindow window(sf::VideoMode(1024, 768), "Arcanoid!");
		window.setFramerateLimit(50);
		window.setVerticalSyncEnabled(true);

		client game(window, ip);
		// Главный цикл приложения
		while (window.isOpen()) {
			// Обрабатываем события в цикле
			sf::Event event;
			while (window.pollEvent(event)) {
				// Кроме обычного способа наше окно будет закрываться по нажатию на Escape
				if (event.type == sf::Event::Closed ||
					(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
					window.close();
			}
			game.run(window);

			// game.update();
		}
 	} else {
/*        int pid = fork();
        if (pid == 0) {
            //run server
        }
        return 0;*/
		//Сервер с логическим миром и физикой пока только для одного игрока
	server serv(2000);

		// Создаем главное окно приложенияc
		sf::RenderWindow window(sf::VideoMode(150,150), "Server!");
		window.setFramerateLimit(50);
		window.setVerticalSyncEnabled(true);
		while (window.isOpen()) {

		sf::Event event;
			while (window.pollEvent(event)) {
				// Кроме обычного способа наше окно будет закрываться по нажатию на Escape
				if (event.type == sf::Event::Closed ||
					(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
					window.close();
			}

			serv.input();
			serv.output();
		}
	}
	return 0;
 }