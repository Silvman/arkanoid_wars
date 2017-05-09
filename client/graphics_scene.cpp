//
// Created by alex on 09.05.17.
//

#include "graphics_scene.hpp"

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
	font.loadFromFile("/home/alex/gittt/arkanoid_wars/client/CyrilicOld.TTF");
	texture_background.loadFromFile("/home/alex/gittt/arkanoid_wars/client/background.jpg");
	background.setTexture(texture_background);
	background.setColor(sf::Color(176,176,176));

	texture_heart.loadFromFile("/home/alex/gittt/arkanoid_wars/client/heart.png");
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

		conect.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 - 30 );
		conect.setString("Problems with connecting \n another player");

		window.draw(conect);
	}

	// Отрисовка
	window.display();
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
	conect.setPosition(window.getSize().x/2 - 280 ,window.getSize().y/2 );
	conect.setString("Waiting other player");
	window.draw(conect);

	window.display();
}