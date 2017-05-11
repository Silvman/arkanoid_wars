//
// Created by alex on 11.05.17.
//

#ifndef ARKANOIDWARS_SOUND_HPP
#define ARKANOIDWARS_SOUND_HPP

#include <SFML/Audio.hpp>


class sound {
private:
	sf::SoundBuffer buffer_hit_player;
	sf::Sound hit_player;

	sf::SoundBuffer buffer_hit_block;
	sf::Sound hit_block;
public:
	sound();
	void run(bool isPlayerKicked, int brokenBlock);
	~sound(){
	};
};


#endif //ARKANOIDWARS_SOUND_HPP
