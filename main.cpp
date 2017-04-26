#include <SFML/Graphics.hpp>

/* Отдельный класс мир( персонаж, мячик, массив блоков)
 */

// класс Игрок (для основной сцены)
class player {
private:
    float x;
    float speed;
    int score, lives;

public:
    player(const float x_start) : x(x_start), speed(0.3), score(0), lives(2) { }

    const float move(float dx) {
        x += dx;
        return x;
    }

    const float getX() {
        return x;
    }

    const float getSpeed() {
        return speed;
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

    ~player() {}
};

// класс Тело игрока (для физической и графической сцен)
class player_body {
private:
    sf::RectangleShape body;

public:
    player_body(
            const float x_start, const float y_start,
            const float height, const float width
    ) {
        sf::Vector2f size(height, width);

        body.setPosition(x_start, y_start);
        body.setSize(size);
        body.setOrigin(height/2, width/2); // хардкод?
    }

    const sf::Vector2f getPosition() {
        return body.getPosition();
    }

    const sf::Vector2f move(const float dx) {
        body.move(dx, 0); // хардкод?
        return body.getPosition();
    }

    ~player_body() {}
};

class ball {
private:
    sf::Vector2f position;
    sf::Vector2f speed;

public:
    // ХАРДКОД!
    ball(const float start_x, const float start_y) : position(start_x, start_y), speed(0.5, 0.5) { }

    const sf::Vector2f getPosition() const {
        return position;
    }

    const sf::Vector2f getSpeed() const {
        return speed;
    }

    const sf::Vector2f move(const float dx, const float dy) {
        position.x += dx;
        position.y += dy;
        return position;
    }

    const sf::Vector2f move(sf::Vector2f dr) {
        position.x += dr.x;
        position.y += dr.y;
        return position;
    }

    ~ball() { }
};

class ball_body {
private:
    sf::CircleShape body;

public:
    ball_body(
            const float x_start, const float y_start,
            const float radius
    ) {
        body.setPosition(x_start, y_start);
        body.setRadius(radius);
        body.setOrigin(radius/2, radius/2); // хардкод?
    }

    const sf::Vector2f getPosition() {
        return body.getPosition();
    }

    const sf::Vector2f move(const float dx, const float dy) {
        body.move(dx, dy); // хардкод?
        return body.getPosition();
    }

    ~ball_body() {}
};

/*
class physics_world {
private:
    ball_body w_ball;
    player_body f_player;
    sf::Vector2f lt_border;
    sf::Vector2f rb_border;

public:
    // ХАРДКОД
    physics_world(
            const float x_start, const float y_start,
            sf::Vector2f left_top_border, sf::Vector2f right_bottom_border
    ) : w_ball(x_start, y_start + 35, 15),
        f_player(x_start, y_start, 100, 10),
        lt_border(left_top_border),
        rb_border(right_bottom_border)
    { }

    void update(float x, float) {

    }

    ~physics_world() {}
};*/

class world {
private:
    player f_player;
    ball w_ball;
    //physics_world pw;
    sf::Clock clock;
    float time;

public:
    world(const float x_start, const float y_start)
            : f_player(x_start),
              w_ball(x_start, y_start + 5) { }

    void update() {
        time = clock.getElapsedTime().asMilliseconds();
        clock.restart();

        float last_pl_position = f_player.getX();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            f_player.move(static_cast<float>(- f_player.getSpeed() * time));
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            f_player.move(static_cast<float>(f_player.getSpeed() * time));
        }



    }

    ~world() { }
};

int main()
{
    // Создаем главное окно приложения
    sf::RenderWindow window(sf::VideoMode(800, 600), "Arcanoid!");

    sf::Clock clock;
    float time;

    window.setVerticalSyncEnabled(true);

	sf::RectangleShape block1;
	block1.setSize(sf::Vector2f(20, 20));
	block1.setOrigin(20, 20);
	block1.setPosition(125, 100);

	sf::RectangleShape block;
	block.setSize(sf::Vector2f(20, 20));
	block.setOrigin(0,0);
	block.setPosition(0,0);


	sf::RectangleShape rectangle;
    rectangle.setSize(sf::Vector2f(100, 10));
    rectangle.setOrigin(50, 5);
    rectangle.setPosition(400, 580);

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

        // Очистка
        window.clear();
        time = clock.getElapsedTime().asMilliseconds();
        clock.restart();
        // Тут будут вызываться функции обновления и отрисовки объектов

        const float left_border(70);
        const float right_border(730);

         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
         {
            // left key is pressed: move our character
            sf::Vector2f move(-time * 0.3, 0);
            rectangle.setPosition(rectangle.getPosition() + move);
         }

         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
         {
             // left key is pressed: move our character
             sf::Vector2f move(time * 0.3, 0);
             rectangle.setPosition(rectangle.getPosition() + move);
         }
         if(rectangle.getPosition().x < left_border) {
             rectangle.setPosition(left_border, 580);
         }


        if(rectangle.getPosition().x > right_border) {
            rectangle.setPosition(right_border, 580);
        }

        window.draw(rectangle);
		window.draw(block);

		window.draw(block1);
		// Отрисовка
        window.display();
    }

    return 0;
}