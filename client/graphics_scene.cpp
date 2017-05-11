//
// Created by alex on 09.05.17.
//

#include "headers/graphics_scene.hpp"
#include "../common.hpp"

graphics_scene::block_body::block_body(const float x_start, const float y_start,
									   const float height, const float width,
									   const float angle)
{
	sf::Vector2f size(height, width);

	block.setPosition(x_start, y_start);
	block.setSize(size);
	block.setOrigin(height / 2, width / 2);
	block.setRotation(angle);
}

void graphics_scene::block_body::draw(sf::RenderWindow& window)
{
	if(!isKicked) {
		window.draw(block);
	}
}

void graphics_scene::block_body::kick()
{
	isKicked = true;
}

graphics_scene::graphics_scene(sf::RenderWindow& window) : window(window) {
	font.loadFromFile("../../client/img/CyrilicOld.TTF");
	texture_background.loadFromFile("../../client/img/background.jpg");
	background.setTexture(texture_background);
	background.setColor(sf::Color(176,176,176));

	texture_panel.loadFromFile("../../client/img/panel.jpg");
	panel.setTexture(texture_panel);
	panel.setScale(1.5,1.5);
	panel.setPosition(window.getSize().x/2 - 300,window.getSize().y/2 - 165);

	texture_again.loadFromFile("../../client/img/return.png");
	again.setTexture(texture_again);
	again.setColor(sf::Color::Black);
	again.setScale(1.1,1.1);
	again.setPosition(window.getSize().x/2 - 180,window.getSize().y/2 + 25);

	texture_menu.loadFromFile("../../client/img/home.png");
	menu.setTexture(texture_menu);
	menu.setColor(sf::Color::Black);
	menu.setScale(1.1,1.1);
	menu.setPosition(window.getSize().x/2 + 35,window.getSize().y/2 + 25);

	win.setFont(font);
	win.setCharacterSize(65);
	win.setColor(sf::Color::Black);

	texture_heart.loadFromFile("../../client/img/heart.png");
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

	// сдвинуть на относительно сервера

	for(int i = 0; i < map_rows; i++) {
		for(int j = 0; j < map_cols; j++){
			if (map_blocks[i][j]) {
				blocks.push_back(block_body(
						map_start_x + 10 + j * map_width + j * map_space,
						map_start_y + i * map_height + i * map_space,
						map_width,
						map_height,
						0.0f // а это угол
				));
			}
		}
	}
}

void graphics_scene::draw(const sf::Vector2f& player_bottom_coords, const sf::Vector2f& player_top_coords,
						  const sf::Vector2f& ball_coords, int block_to_delete, bool connection, int bot_lives,
						  int top_lives, int bot_score, int top_score) {
	window.clear();
	window.draw(background);


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

	if (connection == 0) {

		window.draw(panel);
		conect.setPosition(window.getSize().x/2 - 270 ,window.getSize().y/2 - 30 );
		conect.setString("Problems with connecting \n      another player");

		window.draw(conect);
	}

	// Отрисовка
}

void graphics_scene::draw_er_con() {
	window.clear();

	window.draw(background);
	conect.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 );
	conect.setString("Problems with connection");
	window.draw(conect);

	window.display();
}

void graphics_scene::draw_wait_con()
{
	window.clear();

	window.draw(background);
	window.draw(panel);
	conect.setPosition(window.getSize().x/2 - 220 ,window.getSize().y/2 );
	conect.setString("Waiting other player");
	window.draw(conect);

	window.display();
}

int graphics_scene::draw_win(players winner) {
	again.setColor(sf::Color::Black);
	menu.setColor(sf::Color::Black);

	if (winner == bottom) {
		win.setPosition(window.getSize().x/2 - 255 ,window.getSize().y/2 - 100);
		win.setString("Bottom player won");
	} else {
		win.setPosition(window.getSize().x/2 - 205 ,window.getSize().y/2 - 100);
		win.setString("Top player won");
	}

	if (sf::IntRect(window.getSize().x/2 - 180,window.getSize().y/2 + 25, 95, 90)
			.contains(sf::Mouse::getPosition(window))) {
		again.setColor(sf::Color::White);
		selection = Again;
	}

	if (sf::IntRect(window.getSize().x/2 + 35,window.getSize().y/2 + 25, 95, 90)
			.contains(sf::Mouse::getPosition(window))) {
		menu.setColor(sf::Color::White);
		selection = Menu;
	}

	window.draw(panel);
	window.draw(win);
	window.draw(again);
	window.draw(menu);
	window.display();

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		switch(selection) {
			case Again: {
				return 1;
			}
			case Menu: {
				return 1;
			}
			default:
				return 3;
		}

	return 3;
}