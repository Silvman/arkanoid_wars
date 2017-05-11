//
// Created by alex on 09.05.17.
//  Графическая цена

#ifndef ARKANOIDWARS_GRAPHICS_SCENE_HPP
#define ARKANOIDWARS_GRAPHICS_SCENE_HPP


#include <SFML/Graphics.hpp>
#include <iostream>
#include "../../common.hpp"
// Первый игрок  - бот, 2 - топ, 0 никто

enum button {
	Again = 0,
	Menu
};

class graphics_scene {
private:
	sf::RenderWindow& window;
	sf::Font font;

	sf::Texture player_img;
	sf::Sprite player_bottom;
	sf::Sprite player_top;

	sf::Texture ball_img;
	sf::Sprite ball;

	sf::Text conect;
	sf::Text score;

	sf::Texture texture_background;
	sf::Sprite background;

	sf::Texture texture_heart;
	sf::Sprite heart;

	button selection;
	sf::Texture texture_panel;
	sf::Sprite panel;

	sf::Texture texture_again;
	sf::Sprite again;

	sf::Texture texture_menu;
	sf::Sprite menu;

	sf::Text win;


	class block_body {
	private:
		sf::RectangleShape block;
		bool isKicked;

	public:
		block_body(const float x_start, const float y_start, const float height,
				   const float width, const float angle = 0);
		void draw(sf::RenderWindow& window) ;
		void kick() ;
		void reset() ;
		~block_body() {};
	};

	std::vector<block_body> blocks;

public:
	graphics_scene(sf::RenderWindow& window);
	void draw(const sf::Vector2f& player_bottom_coords, const sf::Vector2f& player_top_coords,
			  const sf::Vector2f& ball_coords, int block_to_delete, bool connection, int bot_lives = 3,
			  int top_lives = 3, int bot_score = 0, int top_score = 0);
	void reset_all_blocks();
    void draw_er_con() ;
	void draw_wait_con();
	int draw_win(players winner);
	~graphics_scene() {}
};


#endif //ARKANOIDWARS_GRAPHICS_SCENE_HPP
