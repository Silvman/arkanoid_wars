
#include "headers/server.hpp"


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

		/*       int pid = fork();
				if (pid == 0) {
					//run server

				return 0;
				*/
		//Сервер с логическим миром и физикой пока только для одного игрока
    server serv(2000);

		// Создаем главное окно приложения сервер
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

	return 0;
}