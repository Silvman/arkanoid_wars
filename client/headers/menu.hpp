//
// Created by alex on 09.05.17.
//

#ifndef ARKANOIDWARS_MENU_HPP
#define ARKANOIDWARS_MENU_HPP

#include "client.hpp"

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
	sound audio;

public:
	menu(sf::RenderWindow& WINDOW);
	void run(sf::IpAddress ip);
	~menu(){};

};

#endif //ARKANOIDWARS_MENU_HPP
