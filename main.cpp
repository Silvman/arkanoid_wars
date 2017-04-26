#include <SFML/Graphics.hpp>

/* Отдельный класс мир( персонаж, мячик, массив блоков)
 */

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