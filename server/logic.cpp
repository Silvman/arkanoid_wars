//
// Created by alex on 09.05.17.
//

#include "headers/logic.hpp"

logic_world::logic_player::logic_player(const float x_start, const float y_start, int lives) :
		position(x_start, y_start),
		score(0),
		lives(lives)
{
}

void
logic_world::logic_player::setPosition(sf::Vector2f new_position) {
	position = new_position;
}

const sf::Vector2f
logic_world::logic_player::getPosition() const {
	return position;
}

void
logic_world::logic_player::setScore(const int new_score) {
	score = new_score;
}

int
logic_world::logic_player::getScore() const {
	return score;
}

void
logic_world::logic_player::setLives(const int new_lives) {
	lives = new_lives;
}

int
logic_world::logic_player::getLives() const {
	return lives;
}

void
logic_world::logic_player::operator++() {
	score++;
}

logic_world::logic_ball::logic_ball(const float start_x, const float start_y)
{

}

void
logic_world::logic_ball::setPosition(sf::Vector2f new_position) {
	position = new_position;
}

const sf::Vector2f
logic_world::logic_ball::getPosition() const {
	return position;
}

/*
logic_world::logic_block::logic_block(const int number, block_type type) :
		number(number),
		type(type)
{
}

void
logic_world::logic_block::setKicked() {
	is_kicked = true;
}

int
logic_world::logic_block::getNumber() const {
	return number;
}
*/

logic_world::logic_world(const float window_size_x, const float window_size_y):
		player_bottom(window_size_x / 2.0f, window_size_y - 25.0f, 4),
		player_top(window_size_x / 2.0f, window_size_y - 25.0f),
		ball(window_size_x / 2.0f, window_size_y - 35.0f),
		count_blocks(34),
		physics(window_size_x, window_size_y),
		who_lost_the_ball(no),
		who_leads_the_ball(bottom),
		bottom_border(window_size_y)
{
	// blocks.push_back(logic_block(1));
	/* TODO: сделать логических блоки, сейчас их в принципе нет*/
}

void
logic_world::update(const num_move key_bottom_move, num_action key_bottom_action,
					const num_move key_top_move, num_action key_top_action)
{

	// шарик ушел за пределы поля
	if (ball.getPosition().y > bottom_border) {
		player_bottom.setLives(player_bottom.getLives() - 1);
		who_lost_the_ball = bottom;
	}

	if (ball.getPosition().y < 0) {
		player_top.setLives(player_top.getLives() - 1);
		who_lost_the_ball = top;
	}

	// жизней меньше нуля
	if (player_bottom.getLives() < 0) {
		winner = top;
		return;
	}


	if (player_top.getLives() < 0) {
		winner = bottom;
		return;
	}

	/*
	// другой вариант логики
	if (count_blocks < 1) {
		if(player_bottom.getScore() > player_top.getScore()) {
			winner = bottom;
		} else {
			winner = top;
		}
		return;
	}
	*/


	switch (physics.getHitman()) {
		case bottom: {
			++player_bottom;
			//--count_blocks;
			break;
		}

		case top: {
			++player_top;
			//--count_blocks;
			break;
		}

		default:
			break;
	}

	if (player_bottom.getScore() > count_blocks / 2) {
		winner = bottom;
	}

	if (player_top.getScore() > count_blocks / 2) {
		winner = top;
	}

	/*
	// другой вариант игровой логики
	switch (physics.getHitman()) {
		case bottom: {
			++player_bottom;
			--count_blocks;
			break;
		}

		case top: {
			++player_top;
			--count_blocks;
			break;
		}

		default:
			break;
	}*/

	/*
	for(auto it_bl = blocks.begin(); it_bl != blocks.end(); it_bl++)
		if(it_bl->getNumber() == broken_block)
			it_bl->setKicked();
	*/

	// обрабатываем кажатые клавишы с учетом того, у кого шарик - у верхнего или у нижнего
	if (who_leads_the_ball == bottom) {
		// если шарик у нижнего, верхний не может его запустить
		if(key_top_action != use_weapon)
			key_top_action = nothing;

	} else {
		if(key_bottom_action != use_weapon)
			key_bottom_action = nothing;

	}

	physics.calculate(key_bottom_move, key_bottom_action,
					  key_top_move, key_top_action,
					  who_lost_the_ball, who_leads_the_ball);

	// шарик отдается тому, кто его упустил
	if(who_lost_the_ball != no)
		who_leads_the_ball = who_lost_the_ball;

	// обработали потерю шарика, можем вернуть переменной исходное значение
	who_lost_the_ball = no;

	player_bottom.setPosition(physics.givePlayerBottomCoords());
	player_top.setPosition(physics.givePlayerTopCoords());
	ball.setPosition(physics.giveBallCoords());

	broken_block = physics.getBrokenBlock();

	sleep(sf::milliseconds(10));
}

const sf::Vector2f
logic_world::givePlayerTopCoords() const {
	return player_top.getPosition();
}

const sf::Vector2f
logic_world::givePlayerBottomCoords() const {
	return player_bottom.getPosition();
}

const sf::Vector2f
logic_world::giveBallCoords() const {
	return ball.getPosition();
}

const sf::Vector2f
logic_world::giveBallSpeed() const {
	return physics.giveBallSpeed();
}

int
logic_world::giveLivesBottom() const {
	return player_bottom.getLives();
}

int
logic_world::giveLivesTop() const {
	return player_top.getLives();
}

int
logic_world::giveScoreBottom() const
{
	return player_bottom.getScore();
}

int
logic_world::giveScoreTop() const
{
	return player_top.getScore();
}

int
logic_world::getBrokenBlocks(){
	int buf = broken_block;
	broken_block = -1;
	return buf;
}

bool
logic_world::checkPlayerKicked()
{
	return physics.checkPlayerKicked();
}

players
logic_world::whoWin() const {
	return winner;
}