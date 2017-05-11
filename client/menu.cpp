//
// Created by alex on 09.05.17.
//

#include "headers/menu.hpp"

menu::menu(sf::RenderWindow& WINDOW) :
window(WINDOW),
CLIENT(WINDOW)
{
	texture_background.loadFromFile("../../client/img/background.jpg");
	background.setTexture(texture_background);
	background.setColor(sf::Color(176,176,176));
	font.loadFromFile("../../client/img/CyrilicOld.TTF");


	start.setFont(font);
	start.setCharacterSize(75);
	start.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 - 100);
	start.setString("Start game");

	end.setFont(font);
	end.setCharacterSize(75);
	end.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 + 100);
	end.setString("Exit");
}

void menu::run(sf::IpAddress ip) {
	audio.background();
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
			if (menuNum == 1) {
				isMenu = 2;
				//CLIENT.askReplay();
			}

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
		isMenu = CLIENT.run(window);
	}

	if (isMenu == 4) {
		CLIENT.askReplay();
		isMenu = 3;
	}
}