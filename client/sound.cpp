//
// Created by alex on 11.05.17.
//

#include "headers/sound.hpp"

sound::sound()
{
	buffer_hit_player.loadFromFile("../../client/img/hit.wav");
	hit_player.setBuffer(buffer_hit_player);
    hit_player.setVolume(25);

	buffer_hit_block.loadFromFile("../../client/img/block.wav");
	hit_block.setBuffer(buffer_hit_block);
    hit_block.setVolume(25);

	music.openFromFile("../../client/img/music.ogg");
	music.setLoop(1);
	music.setVolume(50);
}


void sound::run(bool isPlayerKicked, int brokenBlock)
{
	if (isPlayerKicked == 1 )
		hit_player.play();
	
	
	if (brokenBlock != -1)
		hit_block.play();
}

void sound::background()
{
	if (isMusic == 0)
		music.play();
}