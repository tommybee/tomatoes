/*************************************************************************

                         "I Have No Tomatoes"
                  Copyright (c) 2004, Mika Halttunen

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute
 it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must
    not claim that you wrote the original software. If you use this
    software in a product, an acknowledgment in the product documentation
    would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must
    not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.


 Mika Halttunen <lsoft@mbnet.fi>

*************************************************************************/

#include <stdio.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "special_power.h"
#include "enemy.h"
#include "mymath.h"
#include "effects.h"
#include "particle.h"
#include "texture.h"
#include "tilemap.h"
#include "bomb.h"
#include "game.h"
#include "trap.h"
#include "helpers.h"
#include "soundmusic.h"
#include "select_special.h"


// Special power in progress? (0 == not using, 1 == p1 using, 2 == p2)
int using_special_power = 0;

// Which special power
int which_special_power = 0;

// Special power timer
int special_power_count = 0;

// Does the special power pause the game?
bool special_power_pause;

// Teleport particle from teleport.cpp
extern GLuint part_teleport;


// The power instances
SP_TRAP				sp_trap;
SP_WILDFIRE			sp_wildfire;
SP_NAPALM			sp_napalm;
SP_WISP				sp_wisp;
SP_POTATOMAN		sp_potatoman;
SP_FLOWERPOWER		sp_flowerpower;
SP_TELEPORT			sp_teleport;
SP_TURN				sp_turn;
SP_LIGHTNING		sp_lightning;



// Invoke a special power
void invoke_special_power(int who, int what) {
//	if(using_special_power)
//		return;

	using_special_power = who;
	which_special_power = what;
	special_power_count = 0;
	special_power_pause = true;

	// Initialize the correct special power instance
	switch(which_special_power) {
		default:
		case RED_POWER_TRAP:
			sp_trap.init(); break;
		case RED_POWER_WILDFIRE:
			sp_wildfire.init(); break;
		case RED_POWER_NAPALM:
			sp_napalm.init(); break;
		case GREEN_POWER_WISP:
			sp_wisp.init(); break;
		case GREEN_POWER_POTATOMAN:
			sp_potatoman.init(); break;
		case GREEN_POWER_FLOWERPOWER:
			sp_flowerpower.init(); break;
		case BLUE_POWER_TELEPORT:
			sp_teleport.init(); break;
		case BLUE_POWER_TURN:
			sp_turn.init(); break;
		case BLUE_POWER_LIGHTNING:
			sp_lightning.init(); break;
	}
}


// Draw the special power
void draw_special_power() {
	if(!using_special_power)
		return;

	// Draw the correct special power instance
	switch(which_special_power) {
		default:
		case RED_POWER_TRAP:
			sp_trap.draw(); break;
		case RED_POWER_WILDFIRE:
			sp_wildfire.draw(); break;
		case RED_POWER_NAPALM:
			sp_napalm.draw(); break;
		case GREEN_POWER_WISP:
			sp_wisp.draw(); break;
		case GREEN_POWER_POTATOMAN:
			sp_potatoman.draw(); break;
		case GREEN_POWER_FLOWERPOWER:
			sp_flowerpower.draw(); break;
		case BLUE_POWER_TELEPORT:
			sp_teleport.draw(); break;
		case BLUE_POWER_TURN:
			sp_turn.draw(); break;
		case BLUE_POWER_LIGHTNING:
			sp_lightning.draw(); break;
	}
}

// Update the special power
void update_special_power() {
	if(!using_special_power)
		return;

	special_power_count++;

	// Handle the different special powers
	switch(which_special_power) {
		default:
		case RED_POWER_TRAP:
			sp_trap.update(); break;
		case RED_POWER_WILDFIRE:
			sp_wildfire.update(); break;
		case RED_POWER_NAPALM:
			sp_napalm.update(); break;
		case GREEN_POWER_WISP:
			sp_wisp.update(); break;
		case GREEN_POWER_POTATOMAN:
			sp_potatoman.update(); break;
		case GREEN_POWER_FLOWERPOWER:
			sp_flowerpower.update(); break;
		case BLUE_POWER_TELEPORT:
			sp_teleport.update(); break;
		case BLUE_POWER_TURN:
			sp_turn.update(); break;
		case BLUE_POWER_LIGHTNING:
			sp_lightning.update(); break;
	}
}



// Trap power stuff
////////////////////////////////////////////////////////

// Initialize
void SP_TRAP::init() {
	// Add a trap
	if(using_special_power == 1)
		add_trap((int)p1.get_real_x(), (int)p1.get_real_y(), 1);
	else if(using_special_power == 2)
		add_trap((int)p2.get_real_x(), (int)p2.get_real_y(), 2);


	using_special_power = 0;
	special_power_pause = false;

	// Reduce the power amount
	icon_menu.count[RED_POWER_TRAP]--;

	// Play the trap laying sound
	play_sound(SND_TRAP, false);
}


// Update
void SP_TRAP::update() {

}


// Draw
void SP_TRAP::draw() {

}


// Wildfire power stuff
////////////////////////////////////////////////////////

// From enemy.cpp
extern int enemy_burn_time;

// Initialize
void SP_WILDFIRE::init() {

	// Wildfire doesn't pause the game
	special_power_pause = false;

	if(enemylist.size() == 0) {
		// No enemies, cancel
		using_special_power = 0;
		return;
	}
	else {
		// Choose the first non-burning enemy in the list as the first victim
		bool burnt = false;
		list<ENEMY>::iterator e;
		for(e = enemylist.begin(); e != enemylist.end(); ++e) {
			if(!(*e).burning && !(*e).dying) {
				(*e).burning = true;
				(*e).burn_time = enemy_burn_time;
				(*e).speed += 0.06f;
				burnt = true;
				break;
			}
		}

		// Check that we actually burnt somebody
		if(!burnt) {
			// Cancel
			using_special_power = 0;
			return;
		}
	}

	// Reduce the power amount
	icon_menu.count[RED_POWER_WILDFIRE]--;
	using_special_power = 0;

	// Play the burning sound
	play_sound(SND_WILDFIRE, false);
}


// Update
void SP_WILDFIRE::update() {

}

// Draw
void SP_WILDFIRE::draw() {

}


// Napalm power stuff
////////////////////////////////////////////////////////

// Helper function from player.cpp
void get_random_block_at(int x, int y, int &bx, int &by);


// Initialize
void SP_NAPALM::init() {
	// Save the old player positions before jumping
	oldx[0] = (int)p1.get_real_x();
	oldy[0] = (int)p1.get_real_y();
	oldx[1] = (int)p2.get_real_x();
	oldy[1] = (int)p2.get_real_y();

	// Make the player jump to safety, but don't jump if we're already on a block
	if(!map_solid(oldx[0], oldy[0])) {
		int jtx = 0, jty = 0;
		get_random_block_at(oldx[0], oldy[0], jtx, jty);
		p1.jump(jtx, jty, 2.0f, 0.05f);

		// Play the jump sound
		play_sound(SND_JUMP, false);
	}
	if(!map_solid(oldx[1], oldy[1]) && two_players) {
		int jtx = 0, jty = 0;
		get_random_block_at(oldx[1], oldy[1], jtx, jty);
		p2.jump(jtx, jty, 2.0f, 0.05f);

		// Play the jump sound
		play_sound(SND_JUMP, false);
	}


	// Reduce the power amount
	icon_menu.count[RED_POWER_NAPALM]--;

	// Initialize the bombing raid! :)
	cur_x = 1;
	cur_y = 0;
	explo_delay = 30;
}


// Update
void SP_NAPALM::update() {
	// Reduce the explosion delay
	if(explo_delay > 0)
		explo_delay--;

	// If it's time to create a new explosion, do it
	if(explo_delay == 0) {
		// Create an explosion
		if(!map_solid(cur_x, cur_y)) {
			create_explosion(cur_x, cur_y, EXP_NAPALM);
			explo_delay = 1;

			// Check for enemies here and kill them
			list<ENEMY>::iterator e;
			for(e = enemylist.begin(); e != enemylist.end(); ++e) {
				if((int)((*e).get_real_x()) == cur_x && (int)((*e).get_real_y()) == cur_y && !(*e).dying)
					(*e).die();
			}
		}

		// Move
		cur_x+=2;
		if(cur_x > MAP_W-1) {
			cur_x = (cur_y % 2) ? 1 : 0;
			cur_y++;
			if(cur_y > MAP_H-1) {
				// Jump the players back to where they were
				if(!map_solid(oldx[0], oldy[0])) {
					p1.jump(oldx[0], oldy[0], 2.0f, 0.05f);

					// Play the jump sound
					play_sound(SND_JUMP, false);
				}
				if(two_players && !map_solid(oldx[1], oldy[1])) {
					p2.jump(oldx[1], oldy[1], 2.0f, 0.05f);

					// Play the jump sound
					play_sound(SND_JUMP, false);
				}

				// ..and we're done!
				using_special_power = 0;
				special_power_pause = false;
			}

			// Play the explosion sound
			play_sound(SND_EXPLO, true);
		}
	}

}


// Draw
void SP_NAPALM::draw() {

}


// Wisp power stuff
////////////////////////////////////////////////////////

// Initialize
void SP_WISP::init() {
	// Wisp doesn't pause the game
	special_power_pause = false;

	// If the wisp is already there, cancel
	if(wisp.alive) {
		using_special_power = 0;
		return;
	}


	// Create the wisp
	create_wisp();

	// Reduce the power amount
	icon_menu.count[GREEN_POWER_WISP]--;
	using_special_power = 0;

	// Play the wisp sound
	play_sound(SND_WISP, false);
}


// Update
void SP_WISP::update() {

}

// Draw
void SP_WISP::draw() {

}


// Potato man power stuff
////////////////////////////////////////////////////////

// Initialize
void SP_POTATOMAN::init() {
	// Potato man doesn't pause the game
	special_power_pause = false;

	// If the potato man is already there, cancel
	if(potatoman.alive) {
		using_special_power = 0;
		return;
	}

	// Create the potato man
	create_potatoman();


	// Reduce the power amount
	icon_menu.count[GREEN_POWER_POTATOMAN]--;
	using_special_power = 0;

	// Play the potato man sound
	play_sound(SND_POTATOMAN, false);
}


// Update
void SP_POTATOMAN::update() {

}

// Draw
void SP_POTATOMAN::draw() {

}


// Flower power stuff
////////////////////////////////////////////////////////

// Initialize
void SP_FLOWERPOWER::init() {
	// Check that player isn't reloading and hasn't trown too much bombs
	if(using_special_power == 1 && (p1.reload != 0 || p1.num_flower_bombs >= 3)) {
		// Cancel
		using_special_power = 0;
		special_power_pause = false;
		return;
	}
	else if(using_special_power == 2 && (p2.reload != 0 || p2.num_flower_bombs >= 3)) {
		// Cancel
		using_special_power = 0;
		special_power_pause = false;
		return;
	}

	// Plant the flower bomb
	if(using_special_power == 1) {
		p1.reload = 30;

		add_bomb(p1.x, p1.y, BTYP_FLOWER, 1);
		p1.num_bombs++;
		p1.num_flower_bombs++;
	}
	else if(using_special_power == 2) {
		p2.reload = 30;

		add_bomb(p2.x, p2.y, BTYP_FLOWER, 2);
		p2.num_bombs++;
		p2.num_flower_bombs++;
	}

	// Play the sound
	play_sound(SND_BOMB, false);

	// Reduce the power amount
	icon_menu.count[GREEN_POWER_FLOWERPOWER]--;

	using_special_power = 0;
	special_power_pause = false;
}


// Update
void SP_FLOWERPOWER::update() {

}


// Draw
void SP_FLOWERPOWER::draw() {

}


// Teleport power stuff
////////////////////////////////////////////////////////

// Helper function which gets a random position block to which the player can
// teleport
bool block_teleport(int tx, int ty) {
	if(!map[tx][ty][1])
		return false;

	int count = 0;
	if(!map_solid(tx-1, ty))
		count++;
	if(!map_solid(tx+1, ty))
		count++;
	if(!map_solid(tx, ty-1))
		count++;
	if(!map_solid(tx, ty+1))
		count++;

	if(count)
		return true;
	else
		return false;
}


// Initialize
void SP_TELEPORT::init() {
	who = using_special_power;
	teleport_dir = DIR_UP;
	teleport_pos = 0.0f;

	// Choose the target position
	tx = RAND(0, MAP_W-1);
	ty = RAND(0, MAP_H-1);
	while(!block_teleport(tx, ty)) {
	//while(!can_teleport(tx, ty, true)) {
		tx = RAND(0, MAP_W-1);
		ty = RAND(0, MAP_H-1);
	}

	// Reduce the power amount
	icon_menu.count[BLUE_POWER_TELEPORT]--;

	// Play the teleport sound
	play_sound(SND_TELEPORT, false);
}


// Update
void SP_TELEPORT::update() {
	PLAYER &p = (who == 1) ? p1 : p2;

	// Update the teleport position
	if(teleport_dir == DIR_UP) {
		// Going up
		teleport_pos += 0.015f;

		// If we're on the top
		if(teleport_pos >= 1.0f) {
			teleport_pos = 1.0f;
			teleport_dir = DIR_DOWN;
			p.x = tx;
			p.y = ty;
			p.tx = tx;
			p.ty = ty;
			p.offset = 0;
			p.walking = false;
			p.anim = 0;
		}
	}
	else if(teleport_dir == DIR_DOWN) {
		// Going down
		teleport_pos -= 0.015f;

		// If we're landed
		if(teleport_pos <= 0.0f) {
			teleport_pos = 0.0f;
			using_special_power = 0;
			special_power_pause = false;
		}
	}


	// Create some particles
	VECT pos(p.get_real_x(), 0.25f, p.get_real_y());
	pos.y += TELEPORT_POWER_HEIGHT * teleport_pos;

	VECT dir;
	for(int f=0; f<5; f++) {
		VECT ppos = pos + VECT(RANDF(-0.5f,0.5f),RANDF(-0.5f,0.5f),RANDF(-0.5f,0.5f));
		dir.x = dir.y = dir.z = 0.0f;
		float c1[4] = { 0.3f, 0.3f, 1, 1 };
		float c2[4] = { 0.3f, 1, 1, 0.1f };

		if(ppos.y < TILE_H)
			ppos.y = TILE_H;

		add_particle(ppos, dir, RAND(10,30), 0.3f, 0.1f, c1, c2, part_star);
	}
}


// Draw
void SP_TELEPORT::draw() {

}


// Turn power stuff
////////////////////////////////////////////////////////

// Helper function which creates a new path for the enemy
void create_new_path(ENEMY *e) {
	// Trace in straight line to the opposite direction and create a path to there
	int dir = e->dir + 2;
	if(dir > DIR_W)
		dir -= 4;


	// Movement deltas for each direction
	const int dx[4] = {  0, 1, 0, -1 };
	const int dy[4] = { -1, 0, 1,  0 };

	// Sweep in a straigth line and check for the walls
	int px, py;
	int pos;
	for(pos=0; pos < MAP_W; pos++) {
		px = e->x + dx[dir] * pos;
		py = e->y + dy[dir] * pos;

		// Check for solid tile
		if(map_solid(px, py)) {
			px -= dx[dir];
			py -= dy[dir];
			break;
		}
	}

	// Calculate a new path to (px,py)
	if(e->pf.find_path(e->x, e->y, px, py) == PATH_FAILED) {
		e->path_pos = -1;
		return;
	}

	// Now we've got a nice path for us!
	e->path_pos = 0;
	e->offset = 0.0f;
	e->tx = e->pf.path[0].x;
	e->ty = e->pf.path[0].y;

}


// Initialize
void SP_TURN::init() {
	// Turn doesn't pause the game
	special_power_pause = false;

	if(enemylist.size() == 0) {
		// No enemies, cancel
		using_special_power = 0;
		return;
	}
	else {
		// Begin turning the enemies
		list<ENEMY>::iterator e;
		for(e = enemylist.begin(); e != enemylist.end(); ++e) {
			if(!(*e).dying && !(*e).turning && !(*e).burning) {
				(*e).x = (int)(*e).get_real_x();
				(*e).y = (int)(*e).get_real_y();
				(*e).turning = 1;
				(*e).offset = 0.0f;
				(*e).nextdir = (*e).dir + 1;
				if((*e).nextdir > DIR_W)
					(*e).nextdir = DIR_N;
				(*e).turning_counter = 0;
				(*e).turning_raise = 0.0f;
				(*e).speed = 0.05f;
				(*e).chase = 0;

				create_new_path(&(*e));
			}
		}
	}

	// Reduce the power amount
	icon_menu.count[BLUE_POWER_TURN]--;

	// Play the turn sound
	play_sound(SND_TURN, false);
}


// Update
void SP_TURN::update() {
	// Stop the power after a second
	if(special_power_count > 60) {
		using_special_power = 0;
	}
}


// Draw
void SP_TURN::draw() {

}


// Lightning power stuff
////////////////////////////////////////////////////////

// Initialize
void SP_LIGHTNING::init() {
	// Clear the targets
	for(int f=0; f<ENEMY_AMOUNT; f++) {
		targets[f] = NULL;
		noise[f] = RANDF(0,359);
	}

	// Choose 1 - ENEMY_AMOUNT targets
	num_targets = RAND(1,ENEMY_AMOUNT);
	if(num_targets > (signed)enemylist.size())
		num_targets = enemylist.size();

	list<ENEMY>::iterator e = enemylist.begin();
	int i;
	for(i=0; i<num_targets; i++) {
		targets[i] = &(*e);				// Pointer to the enemy in the list
		if(targets[i]->dying)
			targets[i] = NULL;
		e++;
	}

	// Count the number of the actual targets
	int num = 0;
	for(i=0; i<ENEMY_AMOUNT; i++) {
		if(targets[i])
			num++;
	}

	// If there isn't any, cancel the special power
	if(num == 0) {
		using_special_power = 0;
		special_power_pause = false;
	}
	else {
		// There is valid target(s), reduce the power amount
		icon_menu.count[BLUE_POWER_LIGHTNING]--;

		// Play the lightning sound
		play_sound(SND_LIGHTNING, false);
	}
}


// Update
void SP_LIGHTNING::update() {
	// Animate the noise using sine wave
	for(int f=0; f<ENEMY_AMOUNT; f++) {
		noise[f] = add_angle(noise[f], 5.0f);

		// Create some particles emerging from the targets
		if(RAND(0,100) > 50 && targets[f] && !targets[f]->dying) {
			VECT pos(targets[f]->get_real_x(), 0.5f, targets[f]->get_real_y());
			VECT dir;
			pos.x += RANDF(-0.5f, 0.5f);
			pos.y += RANDF(-0.5f, 0.5f);
			pos.z += RANDF(-0.5f, 0.5f);

			dir.x = RANDF(-0.025f, 0.025f);
			dir.y = RANDF(0.025f, 0.025f);
			dir.z = RANDF(-0.025f, 0.025f);
			float c1[4] = { .2f, .8f, 1, 1 };
			float c2[4] = { 0, 0, 1, 0 };
			add_particle(pos, dir, RAND(10,60), 0.5f, 0.3f, c1, c2, part_glow);
		}
	}

	if(special_power_count > 120) {
		// Kill the targets
		for(int f=0; f<ENEMY_AMOUNT; f++) {
			if(targets[f] && !targets[f]->dying) {
				targets[f]->die();
				targets[f] = NULL;
			}
		}

		using_special_power = 0;
		special_power_pause = false;
	}
}


// Draw
void SP_LIGHTNING::draw() {
	// Draw the lightning bolts to the targets
	int f;
	for(f=0; f<ENEMY_AMOUNT; f++) {
		if(targets[f] && !targets[f]->dying) {
			VECT pos1(targets[f]->get_real_x(), 0.5f, targets[f]->get_real_y());
			VECT pos2 = pos1;
			pos2.y += 14.0f;

			// Compute the noise levels using a sine wave
			float noise1 = 0.35f + (SIN(noise[f]) * 0.2f);
			float noise2 = 0.2f + (COS(noise[f]) * 0.1f);
			draw_lightning(pos1, pos2, noise1, noise2);
		}
	}
}

