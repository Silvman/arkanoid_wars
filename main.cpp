#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

/* Отдельный класс мир( персонаж, мячик, массив блоков)
 */

// класс Игрок (для основной сцены)
class player {
private:
    float x;
    float speed;
    int score, lives;

public:
    player(const float x_start) : x(x_start), speed(0.3), score(0), lives(3) { }

    const float move(float dx) {
        x += dx;
        return x;
    }

    const float getX() const {
        return x;
    }

    const float getSpeed() const {
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

// класс Тело игрока (ТОЛЬКО графической сцен) // Для физики надо длеать на объекте движка box2d
class player_body {
private:
    sf::RectangleShape body;

public:
    player_body(const float x_start, const float y_start, const float height, const float width) {
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

// в Мире должны быть классы только для мира + time . Обработка events в world

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

    b2Vec2 gravity(10.0f, 0.0f);
    b2World world_ph(gravity);

    b2BodyDef left_border_def;
    left_border_def.position.Set(0, 300.0f);
    b2Body* left_border_Body = world_ph.CreateBody(&left_border_def);
    b2PolygonShape left_border_b;
    left_border_b.SetAsBox(10.0f, 300.0f);
    left_border_Body ->CreateFixture(&left_border_b, 0.0f);

    b2BodyDef right_border_def;
    left_border_def.position.Set(800, 300.0f);
    b2Body* right_border_Body = world_ph.CreateBody(&right_border_def);
    b2PolygonShape right_border_b;
    right_border_b.SetAsBox(10.0f, 300.0f);
    right_border_Body ->CreateFixture(&right_border_b, 0.0f);

    b2BodyDef top_border_def;
    top_border_def.position.Set(400, 0);
    b2Body* top_border_Body = world_ph.CreateBody(&top_border_def);
    b2PolygonShape top_border_b;
    top_border_b.SetAsBox(400, 10);
    top_border_Body ->CreateFixture(&top_border_b, 0.0f);

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
    rectangle.setPosition(window.getSize().x / 2, window.getSize().y - 20);

    sf::CircleShape ball;
    ball.setRadius(10);
    ball.setOrigin(5, 5);
    ball.setPosition(window.getSize().x / 2, window.getSize().y - 40);
    bool is_launched = false;
    const float left_border(70);
    const float right_border(window.getSize().x - 70);
    const float top_border(70);
    //sf::Vector2f ball_speed(0.5, -0.5);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(window.getSize().x / 2,  window.getSize().y - 40);
    b2Body* body = world_ph.CreateBody(&bodyDef);

    // Define another box shape for our dynamic body.
    b2CircleShape ball_ph;
    b2Vec2 ball_speed(30, -30);
    ball_ph.m_radius = 10;

    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &ball_ph;

    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = 0.1f;

    // Override the default friction.
    fixtureDef.friction = 0.0f;

    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);

    // Prepare for simulation. Typically we use a time step of 1/60 of a
    // second (60Hz) and 10 iterations. This provides a high quality simulation
    // in most game scenarios.
    int32 velocityIterations = 6;
    int32 positionIterations = 2;


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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !is_launched)
        {
            is_launched = true;
            body->SetLinearVelocity(ball_speed);
        }

        world_ph.Step(time, velocityIterations, positionIterations);
        body->SetLinearVelocity(body->GetLinearVelocity() + b2Vec2(ball_speed.x * time, ball_speed.y * time));

        if(is_launched) {
            b2Vec2 pos = body->GetPosition();
            ball.setPosition(pos.x, pos.y);
        }


         if(rectangle.getPosition().x < left_border) {
             rectangle.setPosition(left_border, 580);
         }


        if(rectangle.getPosition().x > right_border) {
            rectangle.setPosition(right_border, 580);
        }
/*
        if(ball.getPosition().x > right_border) {
            ball_speed.x = -ball_speed.x;
        }

        if(ball.getPosition().y < top_border) {
            ball_speed.y = -ball_speed.y;
        }

        if(ball.getPosition().x < left_border) {
            ball_speed.x = -ball_speed.x;
        }

        if(ball.getPosition().y + 5 == rectangle.getPosition().y - 5) {
            ball_speed.y = -ball_speed.y;
        }
*/
        window.draw(rectangle);
        window.draw(block);
        window.draw(ball);

		window.draw(block1);
		// Отрисовка
        window.display();
    }

    return 0;
}