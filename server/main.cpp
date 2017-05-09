#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <Box2D/Box2D.h>
#include <iostream>

#include "physics.h"


/*
 * TODO : в целом
 *
 * (+/-) Убрать хардкод! Пусть все константы определяются в logic_scene, а остальное от них зависит
 * (+, но есть костыли) Сделать по ООП-шному в каждом классе; разнести по player, ball и т.д. (как в logic_scene)
 *
 * (+) Перенести обработку клавиатуры из физики в logic
 *
 * (+) Сделать массив блоков
 * (+) Сделать удара шарика о блок
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
 * Клиент продолжает посылать данные даже после обрыва соединения. Это надо обрабатывать.
 *
 */


typedef struct data_from_server {
    bool connection;
    sf::Vector2f player_bottom_coords, player_top_coords, ball_coords, ball_speed;
    int broken_block;
    bool isPlayerKicked;
    int score_bottom, score_top, lives_bottom, lives_top;
} data_from_server;

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

    return packet;
}


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
	sf::Text score;
	sf::Texture texture_background;
	sf::Sprite background;
	sf::Texture texture_heart;
	sf::Sprite heart;

    class block_body {
        private:
            sf::RectangleShape block;
            bool isKicked;

        public:
            block_body(const float x_start, const float y_start, const float height, const float width, const float angle = 0)
            {
                sf::Vector2f size(height, width);

                block.setPosition(x_start, y_start);
                block.setSize(size);
                block.setOrigin(height / 2, width / 2);
                block.setRotation(angle);
            }

            void draw(sf::RenderWindow& window) {
                if(!isKicked) {
                    window.draw(block);
                }
            }

            void kick() {
                isKicked = true;
            }

            ~block_body() {};
    };

    std::vector<block_body> blocks;


public:
	graphics_scene(sf::RenderWindow& window) : window(window) {
		// TODO: тут координаты не сходятся с теми, что в logic

		font.loadFromFile("../CyrilicOld.TTF");
		texture_background.loadFromFile("../background.jpg");
		background.setTexture(texture_background);
		background.setColor(sf::Color(176,176,176));

		texture_heart.loadFromFile("../heart.png");
		heart.setTexture(texture_heart);
		heart.setScale(0.6,0.6);

		player_bottom.setPosition(window.getSize().x / 2, window.getSize().y - 20);
		player_bottom.setFillColor(sf::Color::Blue);
		player_bottom.setSize(sf::Vector2f(100, 10));
		player_bottom.setOrigin(50, 5); // хардкод

		conect.setColor(sf::Color::Blue);
		conect.setFont(font);
		conect.setCharacterSize(50);
		conect.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 );

		score.setFont(font);
		score.setCharacterSize(30);

		player_top.setPosition(window.getSize().x / 2, 35);
		player_top.setSize(sf::Vector2f(100, 10));
		player_top.setFillColor(sf::Color::Red);
		player_top.setOrigin(50, 5); // хардкод

		ball.setPosition(window.getSize().x / 2, window.getSize().y - 30);
		ball.setRadius(10);

        //blocks.push_back(block_body(550, 260, 120.0f, 40.0f, 0.0f));
        // blocks.push_back(block_body(670, 260, 120.0f, 40.0f, 0.0f));

        const unsigned cols = 8;
        const unsigned rows = 8;

        bool map[cols][rows] = {
                {1, 0, 1, 0, 1, 0, 1, 0},
                {1, 0, 1, 0, 1, 0, 1, 0},
                {1, 0, 1, 0, 1, 0, 1, 0},
                {1, 0, 1, 0, 1, 0, 1, 0},
                {1, 0, 1, 0, 1, 0, 1, 0},
                {1, 0, 1, 0, 1, 0, 1, 0},
                {1, 0, 1, 0, 1, 0, 1, 0},
                {1, 0, 1, 0, 1, 0, 1, 0}
        };

        const float height = 400 / rows;
        const float width = 400 / cols;
        const float space = 5;
        const float start_x = 250;
        const float start_y = 200;

        for(int i = 0; i < rows; i++) {
            for(int j = 0; j < cols; j++){
                if (map[i][j]) {
                    blocks.push_back(block_body(
                            start_x + j * width + j * space,
                            start_y + i * height + i * space,
                            width,
                            height,
                            0.0f
                    ));
                }
            }
        }

	}


    void draw(const sf::Vector2f& player_bottom_coords, const sf::Vector2f& player_top_coords,
			  const sf::Vector2f& ball_coords, int block_to_delete, bool connection, int bot_lives = 3,
              int top_lives = 3, int bot_score = 0, int top_score = 0)
    {
        window.clear();
        window.draw(background);

		if (connection == 0) {

			conect.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 - 30 );
			conect.setString("Problems with connecting \n another player");

			window.draw(conect);
		}

        std::cout << block_to_delete << std::endl;

        player_bottom.setPosition(player_bottom_coords);
        player_top.setPosition(player_top_coords);
        ball.setPosition(ball_coords);

        for (auto it = blocks.begin(); it != blocks.end(); it++) {
            it->draw(window);
        }

        if(block_to_delete != -1) {
            // std::cout << "Aww! " << block_to_delete << std::endl;
            blocks[block_to_delete].kick();
        }

		for (int i = 0; i < bot_lives; i++){
			heart.setColor(sf::Color::Blue);
			heart.setPosition(15,410 + i * 25);
			window.draw(heart);
		}

		for (int i = 0; i < top_lives; i++){
			heart.setColor(sf::Color::Red);
			heart.setPosition(15,290 + i * 25);
			window.draw(heart);
		}


		score.setString(std::to_string(bot_score));
		score.setPosition(window.getSize().x - 75, 450);
		score.setColor(sf::Color::Blue);
		window.draw(score);

		score.setString(std::to_string(top_score));
		score.setColor(sf::Color::Red);
		score.setPosition(window.getSize().x - 75, 330);
		window.draw(score);

        window.draw(player_bottom);
        window.draw(player_top);
        window.draw(ball);

		// Отрисовка
		window.display();
	}

	void draw_er_con() {
		window.clear();

		window.draw(background);
		conect.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 );
		conect.setString("Problems with connection");
		window.draw(conect);

		window.display();
	}

	void draw_wait_con(){

		window.clear();

		window.draw(background);
		conect.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 );
		conect.setString("Waiting other player");
		window.draw(conect);

		window.display();
	}

	~graphics_scene() {}
};

class client {
private:
    sf::Clock clock;
    float dt;

    unsigned my_number;
	bool connection;
    unsigned int key_move, key_action;

    graphics_scene graphics;

    sf::TcpSocket socket;//программный интерфейс для обеспечения обмена данными между процессами
    sf::Packet INPUT;	//Для осуществления пакетной передачи дынных
	sf::Packet OUTPUT;

    data_from_server from_server;

    bool playerKicked;

    /* еще инфа о блоках будет */

public:
	client(sf::RenderWindow& window) : graphics(window), key_move(0), key_action(0)
	{
        
	}

	void connect (sf::IpAddress ip) {
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

		// std::cout << "key_move: " << key_move << ", key_action: " << key_action << std::endl;
		OUTPUT << my_number << key_move << key_action;
		socket.send(OUTPUT);
		OUTPUT.clear();

        dt = clock.getElapsedTime().asSeconds();
        clock.restart();

        if (socket.receive(INPUT) == sf::Socket::NotReady) {
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
        } else {
			// std::cout << "Packet recieved"<< std::endl;
            INPUT >> from_server;



        	std::cout << "player bottom: (" << from_server.player_bottom_coords.x << ", " << from_server.player_bottom_coords.y << ")" << std::endl;
        	std::cout << "player top: (" << from_server.player_top_coords.x << ", " << from_server.player_top_coords.y << ")" << std::endl;
        	std::cout << "ball: (" << from_server.ball_coords.x << ", " << from_server.ball_coords.y << ")" << std::endl;

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

		}

		sleep(sf::milliseconds(10));
	}

	void disconnect() {
		socket.disconnect();
	}

	~client() {
		socket.disconnect();
	}
};

/* ----------------------------- */



/* ------ Серверная часть ------ */

// логика игры
class logic_world {
private:

    class logic_player {
        private:
            sf::Vector2f position;
            int score, lives;

        public:
            logic_player(const float x_start, const float y_start, int lives = 3) : position(x_start, y_start), score(0), lives(lives) { }

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

    class logic_block {
    private:
        enum block_type {
            simple = 0,
            rocket,
            slow_player,
            fast_player,
            freeze_enemy,
            fast_ball
        };

        bool is_kicked;
        int number;
        block_type type;

    public:
        logic_block(const int number, block_type type = simple): number(number), type(type) { }

        void setKicked() {
            is_kicked = true;
        }

        int getNumber() const {
            return number;
        }

        ~logic_block() {}
    };

    // (1) первый игрок - снизу
    logic_player player_bottom;

    // (2) второй игрок - сверху
    logic_player player_top;

    // мячик
    logic_ball ball;

    std::vector<logic_block> blocks;
    int broken_block;

    // Флаг, содержащий информацию о том, кто потерял шарик, и (== 0) если шарик не потерян
    int who_lost_the_ball;
    int who_leads_the_ball;

    physics_scene physics;

    const float bottom_border;


public:
    logic_world(const float window_size_x,const float window_size_y)
            : player_bottom(window_size_x / 2.0f, window_size_y - 25.0f, 4),
              player_top(window_size_x / 2.0f, window_size_y - 25.0f),
              ball(window_size_x / 2.0f, window_size_y - 35.0f),
              physics(window_size_x, window_size_y),
              who_lost_the_ball(0),
              who_leads_the_ball(1),
              bottom_border(window_size_y)
    {
        blocks.push_back(logic_block(1));
        blocks.push_back(logic_block(2));
        /* TODO: сделать логических блоки, сейчас их в принципе нет */
    }

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

        switch (physics.getHitman()) {
            case player_bottom_id: {
                ++player_bottom;
                break;
            }

            case player_top_id: {
                ++player_top;
                break;
            }

            default:
                break;
        }


        for(auto it_bl = blocks.begin(); it_bl != blocks.end(); it_bl++)
            if(it_bl->getNumber() == broken_block)
                it_bl->setKicked();

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

        // шарик отдается тому, кто его упустил
        if(who_lost_the_ball != 0)
            who_leads_the_ball = who_lost_the_ball;

        // обработали потерю шарика, можем вернуть переменной исходное значение
        who_lost_the_ball = 0;

        player_bottom.setPosition(physics.givePlayerBottomCoords());
        player_top.setPosition(physics.givePlayerTopCoords());
        ball.setPosition(physics.giveBallCoords());

        broken_block = physics.getBrokenBlock();

        sleep(sf::milliseconds(10));
    }

	const sf::Vector2f givePlayerTopCoords() const {
		return player_top.getPosition();
	}

    const sf::Vector2f givePlayerBottomCoords() const {
        return player_bottom.getPosition();
    }

	const sf::Vector2f giveBallCoords() const {
		return ball.getPosition();
	}

    const sf::Vector2f giveBallSpeed() const {
        return physics.giveBallSpeed();
    }

    int giveLivesBottom() const {
        return player_bottom.getLives();
    }

    int giveLivesTop() const {
        return player_top.getLives();
    }

    int giveScoreBottom() const {
        return player_bottom.getScore();
    }

    int giveScoreTop() const {
        return player_top.getScore();
    }

    int getBrokenBlocks() {
        int buf = broken_block;
        broken_block = -1;
        return buf;
    }

    bool checkPlayerKicked() {
        return physics.checkPlayerKicked();
    }

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

    data_from_server to_client;
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

            to_client.player_bottom_coords  = serv_world.givePlayerBottomCoords();
            to_client.player_top_coords     = serv_world.givePlayerTopCoords();
            to_client.ball_coords           = serv_world.giveBallCoords();
            to_client.ball_speed            = serv_world.giveBallSpeed();
            to_client.broken_block          = serv_world.getBrokenBlocks();
            to_client.isPlayerKicked        = serv_world.checkPlayerKicked();
            to_client.score_bottom          = serv_world.giveScoreBottom();
            to_client.score_top             = serv_world.giveScoreTop();
            to_client.lives_bottom          = serv_world.giveLivesBottom();
            to_client.lives_top             = serv_world.giveLivesTop();

            connection = 1;
            to_client.connection = 1;

            OUTPUT << to_client;

			socket_1.send(OUTPUT);
			socket_2.send(OUTPUT);
			OUTPUT.clear();
		} else {
            to_client.connection = connection;

            OUTPUT << to_client;
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

class menu {
private:
	sf::Texture texture_background;
	sf::Sprite background;
	sf::RenderWindow& window;
	sf::Font font;
	sf::Text start;
	sf::Text end;
	int menuNum;
	int isMenu = 1;
	client CLIENT;

public:
	menu(sf::RenderWindow& WINDOW) :
			window(WINDOW),
			CLIENT(WINDOW)
	{
		texture_background.loadFromFile("../background.jpg");
		background.setTexture(texture_background);
		background.setColor(sf::Color(176,176,176));
		font.loadFromFile("../CyrilicOld.TTF");


		start.setFont(font);
		start.setCharacterSize(75);
		start.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 - 100);
		start.setString("Start game");

		end.setFont(font);
		end.setCharacterSize(75);
		end.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 + 100);
		end.setString("Exit");
	}

	void run(sf::IpAddress ip) {
		if (isMenu == 1) {
			end.setColor(sf::Color::Blue);
			start.setColor(sf::Color::Blue);
			menuNum = 0;

			window.clear();
			window.draw(background);

			if (sf::IntRect(window.getSize().x / 2 - 280, window.getSize().y / 2 - 75, 375, 75)
					.contains(sf::Mouse::getPosition(window))) {
				start.setColor(sf::Color::White);
				menuNum = 1;
			}

			if (sf::IntRect(window.getSize().x / 2 - 280, window.getSize().y / 2 + 100, 150, 75)
					.contains(sf::Mouse::getPosition(window))) {
				end.setColor(sf::Color::Red);
				menuNum = 2;
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				if (menuNum == 1)
					isMenu = 2;

				if (menuNum == 2)
					window.close();
			}
			window.draw(start);
			window.draw(end);

			window.display();
		}


		if (isMenu == 2) {
			CLIENT.connect(ip);
			isMenu = 3;
		}

		if (isMenu == 3) {
			CLIENT.run(window);
		}
	}

	~menu(){};
};

/* ----------------------------- */



int main() {

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();	//Локальный ip Адресс
	std::cout << ip << std::endl;
	char mode;
	std::cin >> mode;
	if (mode == 'c') {
		std::cin >> ip;
		// Создаем главное окно приложения
		sf::RenderWindow window(sf::VideoMode(1024, 768), "Arcanoid!");
		window.setFramerateLimit(50);
		window.setVerticalSyncEnabled(true);

		menu Menu(window);
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
			Menu.run(ip);

			// game.update();
		}
	} else {
		/*       int pid = fork();
				if (pid == 0) {
					//run server

				return 0;
				*/
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