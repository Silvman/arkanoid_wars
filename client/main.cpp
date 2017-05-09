#include "menu.hpp"
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

int main() {

	sf::IpAddress ip = sf::IpAddress::getLocalAddress();	//Локальный ip Адресс
	std::cout << ip << std::endl;

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
	}

	return 0;
}