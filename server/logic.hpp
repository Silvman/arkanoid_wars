//
// Created by alex on 09.05.17.
//

#ifndef AW_SERVER_LOGIC_HPP
#define AW_SERVER_LOGIC_HPP
#include "physics.hpp"


class logic_world {
private:

	class logic_player {
	private:
		sf::Vector2f position;
		int score, lives;

	public:
		logic_player(const float x_start, const float y_start, int lives = 3);
		void setPosition(sf::Vector2f new_position);
		const sf::Vector2f getPosition() const;
		void setScore(const int new_score);
		int getScore() const;
		void setLives(const int new_lives);
		int getLives() const;
		void operator++();
		~logic_player() {}
	};

	class logic_ball {
	private:
		sf::Vector2f position;

	public:
		logic_ball(const float start_x, const float start_y);
		void setPosition(sf::Vector2f new_position);
		const sf::Vector2f getPosition() const;
		~logic_ball() {}
	};

	class logic_block {
	private:
		enum block_type {
			simple = 0,
			rocket,
			slow_player,
			fast_player,
			freeze_enemy,
			fast_ball
		};

		bool is_kicked;
		int number;
		block_type type;

	public:
		logic_block(const int number, block_type type = simple);
		void setKicked();
		int getNumber() const;
		~logic_block() {}
	};

	// (1) первый игрок - снизу
	logic_player player_bottom;

	// (2) второй игрок - сверху
	logic_player player_top;

	// мячик
	logic_ball ball;

	std::vector<logic_block> blocks;

	int broken_block;
	// Флаг, содержащий информацию о том, кто потерял шарик, и (== 0) если шарик не потерян
	int who_lost_the_ball;
	int who_leads_the_ball;

	physics_scene physics;

	const float bottom_border;

public:
	logic_world(const float window_size_x,const float window_size_y);
	void update(const unsigned int key_bottom_move, unsigned int key_bottom_action,
			const unsigned int key_top_move, unsigned int key_top_action);
	const sf::Vector2f givePlayerTopCoords() const ;
	const sf::Vector2f givePlayerBottomCoords() const;
	const sf::Vector2f giveBallCoords() const;
	const sf::Vector2f giveBallSpeed() const;
	int giveLivesBottom() const;
	int giveLivesTop() const;
	int giveScoreBottom() const;
	int giveScoreTop() const;
	int getBrokenBlocks();
	bool checkPlayerKicked();
	~logic_world() {}

};


#endif //AW_SERVER_LOGIC_HPP
