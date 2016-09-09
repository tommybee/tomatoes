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
#include "texture.h"
#include "game.h"
#include "init.h"
#include "player.h"
#include "mymath.h"
#include "tilemap.h"
#include "enemy.h"
#include "particle.h"
#include "bonus.h"
#include "helpers.h"
#include "effects.h"
#include "special_power.h"
#include "trap.h"
#include "soundmusic.h"
#include "levels.h"


// Enemy list
list<ENEMY> enemylist;

// Enemy animations
GLuint enemy_anim[3];
static int anim_frames[4] = { 0, 1, 0, 2 };


// Texture from player.cpp
extern GLuint sprite_shadow;

// From bonus.cpp
extern int killed_5_diamonds;


// The counter for the continuous enemy creation
int enemy_creation_counter;

// Amount of time between two enemy creation periods
const int enemy_creation_interval = 90;

// Enemy burning time
int enemy_burn_time = 420;


// Load the enemies
void load_enemies() {

	// Load the red enemy animations
	enemy_anim[0] = load_png("enemy1.png", true, false, false);
	enemy_anim[1] = load_png("enemy2.png", true, false, false);
	enemy_anim[2] = load_png("enemy3.png", true, false, false);

}


// Helper function which checks if the players are right next to
// a location or in the same region
bool is_player_close(int x, int y) {

	// Check distance to the players
	int dx = x - (int)p1.get_real_x();
	int dy = y - (int)p1.get_real_y();
	if(dx*dx + dy*dy <= 49)		// 49 == 7*7
		return true;

	if(two_players) {
		dx = x - (int)p2.get_real_x();
		dy = y - (int)p2.get_real_y();
		if(dx*dx + dy*dy <= 49)	// 49 == 7*7
			return true;
	}


	// Determine the regions (4x4)
	int p1reg = 0, p2reg = 0;
	if(p1.x < 7 && p1.y <= 7)
		p1reg = 1;
	else if(p1.x < 7 && p1.y > 7)
		p1reg = 3;
	else if(p1.x >= 7 && p1.y <= 7)
		p1reg = 2;
	else
		p1reg = 4;


	if(two_players) {
		if(p2.x < 7 && p2.y <= 7)
			p2reg = 1;
		else if(p2.x < 7 && p2.y > 7)
			p2reg = 3;
		else if(p2.x >= 7 && p2.y <= 7)
			p2reg = 2;
		else
			p2reg = 4;
	}

	int ereg;
	if(x < 7 && y <= 7)
		ereg = 1;
	else if(x < 7 && y > 7)
		ereg = 3;
	else if(x >= 7 && y <= 7)
		ereg = 2;
	else
		ereg = 4;

	// Check the region
	if(p1reg == ereg)
		return true;
	if(two_players && p2reg == ereg)
		return true;

	return false;
}


// Helper function which adds an new enemy while checking
// for bad places
bool new_enemy(int &ex, int &ey) {
	// Add a new enemy
	ENEMY e;
	e.clear();

	// Check for bad places
	int x = RAND(0, MAP_W-1);
	int y = RAND(0, MAP_H-1);
	int counter = 0;
	while((!can_teleport(x, y) || is_player_close(x,y)) && counter < 10000) {
		x = RAND(0, MAP_W-1);
		y = RAND(0, MAP_H-1);
		counter++;
	}

	if(counter >= 10000)
		return false;

	e.x = x;
	e.y = y;
	ex = x;
	ey = y;
	e.tx = e.x;
	e.ty = e.y;
	e.type = RAND(0,2);
	e.alive = true;
	enemylist.push_back(e);
	return true;
}


// Move the enemies
void move_enemies() {
	// Create new enemies if there's less than enemy_amount of them
	// (but don't create enemies until all the diamonds from Killed-5 have
	//  been collected)
	int num_enemies = enemylist.size();
	if(num_enemies < ENEMY_AMOUNT && !killed_5_diamonds && !level_pause) {
		enemy_creation_counter--;
		if(enemy_creation_counter < 0) {
			bool created = false;
			// Add enemies to maintain the certain enemy amount
			for(int i=0; i < (ENEMY_AMOUNT - num_enemies); i++) {
				// Add a new enemy
				int ex, ey;
				if(new_enemy(ex, ey)) {
					// Create the teleport effect
					create_teleport_effect(ex, ey);
					created = true;
				}
			}

			// Play the appear sound
			if(created) {
				play_sound(SND_APPEAR, false);
				enemy_creation_counter = enemy_creation_interval;
			}
		}

	}

	// Move the active enemies
	if(enemylist.size() == 0)
		return;

	list<ENEMY>::iterator i;
	for(i = enemylist.begin(); i != enemylist.end(); ++i) {
		(*i).move();
		// Remove the dead enemies
		if((*i).alive == false) {
			i = enemylist.erase(i);
		}
	}

}


// Draw the enemies
void draw_enemies() {
	if(enemylist.size() == 0)
		return;

	list<ENEMY>::iterator i;
	for(i = enemylist.begin(); i != enemylist.end(); ++i) {
		(*i).draw();
	}
}


// Clear the enemy list
void clear_enemies() {
	enemylist.clear();
	enemy_creation_counter = 30;
}


// Helper function which computes the right direction
int get_dir(int dx, int dy) {
	if(dx == 0 && dy == -1)
		return DIR_N;
	else if(dx == 1 && dy == 0)
		return DIR_E;
	else if(dx == 0 && dy == 1)
		return DIR_S;
	else
		return DIR_W;
}


// Look for a player and chase him
void ENEMY::look_player() {
	// Movement deltas for each direction
	const int dx[4] = {  0, 1, 0, -1 };
	const int dy[4] = { -1, 0, 1,  0 };

	// Sweep in a straigth line and check for the player presence
	for(int pos=1; pos < MAP_W; pos++) {
		int xx, yy;
		xx = x + dx[dir] * pos;
		yy = y + dy[dir] * pos;

		// Check for solid tile
		if(map_solid(xx, yy))
			break;

		// Check for the players
		if((int)p1.get_real_x() == xx && (int)p1.get_real_y() == yy && p1.alive && !p1.jumping && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT)) {
			// Begin the chase
			chase = 1;			// 1 for player one
			speed += 0.03f;
			break;
		}
		else if(two_players && (int)p2.get_real_x() == xx && (int)p2.get_real_y() == yy && p2.alive && !p2.jumping && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT)) {
			// Begin the chase
			chase = 2;			// 2 for player two
			speed += 0.03f;
			break;
		}
	}

}


// Lightning special power instance from special_power.cpp
extern SP_LIGHTNING sp_lightning;


// Move the enemy
void ENEMY::move() {
	// Advance the animation
	if(!chase && !burning)
		anim += 0.10f;
	else
		anim += 0.20f;
	if((int)anim > 3 || kicked)
		anim = 0.0f;

	// Advance the dying animation if we're actually dying
	if(dying) {
		die_anim -= 0.03f;

		// Create the blue "burning down" effect
		float px = get_real_x();
		float py = get_real_y();
		for(int f=0; f < RAND(2,10); f++) {
			float rnd = RANDF(-0.3f, 0.3f);
			VECT pos(px, 2*size - 0.05f - (2.5f*size*(1-die_anim)), py);
			pos.x += rnd;
			pos.z -= rnd;
			if(pos.y < 0.0f)
				pos.y = 0.0f;
			if(turning)
				pos.y += (turning_raise * 0.85f);
			VECT dir = 0.0f;
			float c1[4] = { 0.1f, 0.7f, 1, 1 };
			float c2[4] = { 0.1f, 0.7f, 1, 0 };
			add_particle(pos, dir, RAND(20,35), 0.1f, 0.4f, c1, c2, part_star);
		}

		if(die_anim < 0.0f) {
			die_anim = 0.0f;
			alive = false;
		}

		return;
	}


	// Create some particle fire from the burning enemies
	if(burning) {
		VECT ppos(get_real_x(), 0.5f, get_real_y());
		create_fire(ppos);
	}

	// Advance the turning animation
	if(turning) {
		// Raise up
		if(turning == 1) {
			turning_raise += 0.035f;
			if(turning_raise >= 1.0f) {
				turning_raise = 1.0f;
				turning++;
			}
		}
		// Turn
		else if(turning == 2) {
			turning_counter++;
			if(turning_counter == 5) {
				dir = nextdir;
				nextdir = dir + 1;
				if(nextdir > DIR_W)
					nextdir = DIR_N;
			}
			else if(turning_counter == 10) {
				dir = nextdir;
				turning++;
			}
		}
		// Go down
		else if(turning == 3) {
			turning_raise -= 0.035f;
			if(turning_raise <= 0.0f) {
				turning_raise = 0.0f;
				turning = 0;
			}
		}

		// Check the collision between the player #1
		if(p1.alive && !p1.jumping) {
			float dx = get_real_x() - p1.get_real_x();
			float dy = get_real_y() - p1.get_real_y();
			if(dx*dx + dy*dy <= 0.9f) {
				// Collision happened!

				// Kill the player and die
				p1.die();
				die();
			}
		}

		// Check the collision between the player #2
		if(alive && two_players && p2.alive && !p2.jumping) {
			float dx = get_real_x() - p2.get_real_x();
			float dy = get_real_y() - p2.get_real_y();
			if(dx*dx + dy*dy <= 0.9f) {
				// Collision happened!

				// Kill the player and die
				p2.die();
				die();
			}
		}

		return;
	}


	// If there is the lightning special power in progress, don't move the enemies which are
	// suffering from the lightning strikes
	if(special_power_pause && which_special_power == BLUE_POWER_LIGHTNING) {
		// Check if we're a target
		for(int f=0; f<ENEMY_AMOUNT; f++) {
			if(sp_lightning.targets[f] == this) {
				anim += 0.30f;
				if((int)anim > 3)
					anim = 0.0f;
				return;
			}
		}
	}

	// Don't move if the level is finished
	if(level_pause)
		return;


	// Check the traps
	if(traplist.size() > 0) {
		list<TRAP>::iterator t;
		for(t = traplist.begin(); t != traplist.end(); ++t) {
			if(x == (*t).x && y == (*t).y) {
				die();
				return;
			}
		}
	}


	// Handle the burning, that is run around aimlessly
	if(burning) {
		if(!kicked) {
			// Reduce the burning time
			burn_time--;
			if(burn_time == 0) {
				die();
				return;
			}

			// Choose a random direction
			if(RAND(0,100) > 50 && offset == 0.0f) {
				if(RAND(0,100) > 50)
					dir++;
				else
					dir--;
				if(dir > DIR_W)
					dir = DIR_N;
				else if(dir < DIR_N)
					dir = DIR_W;
			}

			// Move one step
			if(tx == x && ty == y) {
				offset = 0.0f;

				// Don't stop until there's a wall.
				switch(dir) {
					default:
					case DIR_N: tx = x; ty = y - 1; break;
					case DIR_E: tx = x + 1; ty = y; break;
					case DIR_S: tx = x; ty = y + 1; break;
					case DIR_W: tx = x - 1; ty = y; break;
				}

				// Check if the target is passable?
				if(map_solid(tx, ty)) {
					// Stop and choose a new dir
					tx = x;
					ty = y;

					dir += RAND(-1,1);
					if(dir < DIR_N)
						dir = DIR_W;
					else if(dir > DIR_W)
						dir = DIR_N;
					return;
				}

			}

			// Move towards the target tile
			if(offset < 1.0f && (tx != x || ty != y)) {
				offset += speed;

				// Check the collision between the player #1
				if(p1.alive && !p1.jumping && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT)) {
					float dx = get_real_x() - p1.get_real_x();
					float dy = get_real_y() - p1.get_real_y();
					if(dx*dx + dy*dy <= 0.9f) {
						// Collision happened!

						// Turn around and run
						tx = x;
						ty = y;
						offset = 0.0f;
						dir += 2;
						if(dir > DIR_W)
							dir -= 4;
					}
				}

				// Check the collision between the player #2
				if(two_players && p2.alive && !p2.jumping && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT)) {
					float dx = get_real_x() - p2.get_real_x();
					float dy = get_real_y() - p2.get_real_y();
					if(dx*dx + dy*dy <= 0.9f) {
						// Collision happened!

						// Turn around and run
						tx = x;
						ty = y;
						offset = 0.0f;
						dir += 2;
						if(dir > DIR_W)
							dir -= 4;
					}
				}

				// Check the collision between the potato men
				if(potatoman.collide_with(this) && !kicked) {
					// Potatoman "kicked" us
					potatoman.kick(this);
				}


				// If we're reached the target tile, move again
				if(offset >= 1.0f) {
					x = tx;
					y = ty;
					offset = 0.0f;
				}
			}
		}

		// Check collisions with other enemies and spread the fire
		bool burnt_somebody = false;
		list<ENEMY>::iterator e;
		for(e = enemylist.begin(); e != enemylist.end(); ++e) {
			if(this != &(*e) && !(*e).burning) {
				// Check the distance
				float dx = get_real_x() - (*e).get_real_x();
				float dy = get_real_y() - (*e).get_real_y();
				if(dx*dx + dy*dy <= 0.9f) {
					// Burn the other enemy
					(*e).burning = true;
					(*e).burn_time = enemy_burn_time;
					(*e).speed += 0.06f;
					burnt_somebody = true;
				}
			}
		}

		// Play the burning sound
		if(burnt_somebody)
			play_sound(SND_WILDFIRE, false);

		if(!kicked)
			return;
	}


	// Not burning below here

	// Choose a random destination
	if(path_pos == -1) {
		// Choose a valid target
		int dx = RAND(0, MAP_W-1);
		int dy = RAND(0, MAP_H-1);
		while(map_solid(dx, dy) || dx == x || dy == y) {
			dx = RAND(0, MAP_W-1);
			dy = RAND(0, MAP_H-1);
		}

		// Calculate the path
		if(pf.find_path(x, y, dx, dy) == PATH_FAILED) {
			// Well, tough luck. We'll just wait and try again later.
			return;
		}

		// Now we've got a nice path for us!
		path_pos = 0;
		offset = 0.0f;
		tx = pf.path[0].x;
		ty = pf.path[0].y;
		dir = get_dir(tx - x, ty - y);
		look_player();
	}

	// Move one step
	if(tx == x && ty == y && path_pos > -1) {
		offset = 0.0f;

		// Follow the path if we're not chasing
		if(chase == 0 && !kicked) {
			path_pos++;
			tx = pf.path[path_pos].x;
			ty = pf.path[path_pos].y;
			dir = get_dir(tx - x, ty - y);
			look_player();
		}
		else if(chase && !kicked) {
			// We are chasing. Don't stop until there's a wall.
			switch(dir) {
				default:
				case DIR_N: tx = x; ty = y - 1; break;
				case DIR_E: tx = x + 1; ty = y; break;
				case DIR_S: tx = x; ty = y + 1; break;
				case DIR_W: tx = x - 1; ty = y; break;
			}

			// Check if the target is passable?
			if(map_solid(tx, ty)) {
				// Stop and choose a new path
				tx = x;
				ty = y;

				path_pos = -1;
				chase = 0;
				speed -= 0.03f;
			}
		}
		else if(kicked) {
			// Potatoman has kicked us. "Fly" straight until we hit a wall.
			switch(dir) {
				default:
				case DIR_N: tx = x; ty = y - 1; break;
				case DIR_E: tx = x + 1; ty = y; break;
				case DIR_S: tx = x; ty = y + 1; break;
				case DIR_W: tx = x - 1; ty = y; break;
			}

			// Check for the wall
			if(map_solid(tx, ty)) {
				die();
				return;
			}
		}
	}

	// Move towards the target tile
	if(offset < 1.0f && (tx != x || ty != y) && path_pos > -1) {
		offset += speed;

		// Check the collision between the player #1
		if(p1.alive && !p1.jumping && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT)) {
			float dx = get_real_x() - p1.get_real_x();
			float dy = get_real_y() - p1.get_real_y();
			if(dx*dx + dy*dy <= 0.9f) {
				// Collision happened!

				// Kill the player and die
				p1.die();
				die();
			}
		}

		// Check the collision between the player #2
		if(alive && two_players && p2.alive && !p2.jumping && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT)) {
			float dx = get_real_x() - p2.get_real_x();
			float dy = get_real_y() - p2.get_real_y();
			if(dx*dx + dy*dy <= 0.9f) {
				// Collision happened!

				// Kill the player and die
				p2.die();
				die();
			}
		}


		// Check the collision between the potato men
		if(potatoman.collide_with(this) && !kicked) {
			// Potatoman "kicked" us
			potatoman.kick(this);
		}


		// If we're reached the target tile, move again
		if(offset >= 1.0f) {
			x = tx;
			y = ty;
			offset = 0.0f;

			// If this is the final destination, stay put and choose a new path
			// on the next cycle
			if(x == pf.dx && y == pf.dy && !chase && !kicked)
				path_pos = -1;
		}
	}

}


// Kill the enemy
void ENEMY::die() {
	if(dying)
		return;		// Hey, don't you die twice, man! ;)
	dying = true;
	kill_count++;
	die_anim = 1.0f;

	// Create a bonus
	add_bonus((int)get_real_x(), (int)get_real_y(), type);

	// Play the sound
	static int last_sound = -1;
	int sound = last_sound;
	while(sound == last_sound)
		sound = RAND(SND_DIE1, SND_DIE6);
	play_sound(sound, false);
	last_sound = sound;
}


// Get current x with offset
float ENEMY::get_real_x() {
	// Calculate the offset
	float offx = 0;
	if(dir == DIR_E)
		offx = offset;
	else if(dir == DIR_W)
		offx = -offset;

	return (float)x + offx + 0.5f;
}


// Get current y with offset
float ENEMY::get_real_y() {
	// Calculate the offset
	float offy = 0;
	if(dir == DIR_N)
		offy = -offset;
	else if(dir == DIR_S)
		offy = offset;

	return (float)y + offy + 0.5f;
}


// Draw the enemy
void ENEMY::draw() {
	// Calculate the offset
	float offx = 0, offz = 0;
	switch(dir) {
		default:
		case DIR_N: offz = -offset; break;
		case DIR_E: offx = offset; break;
		case DIR_S: offz = offset; break;
		case DIR_W: offx = -offset; break;
	}

	// Translate to the position
	glPushMatrix();
	glTranslatef(x + offx + 0.5f, size - 0.20f, y + offz + 0.5f);

	// Draw the shadow
	glDepthMask(GL_FALSE);
	glColor3f(1,1,1);
	BIND_TEXTURE(sprite_shadow);
	float sh = -(size - 0.21f);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 0.6f, sh, -0.6f);
		glTexCoord2f(0,1); glVertex3f(-0.6f, sh, -0.6f);
		glTexCoord2f(1,0); glVertex3f( 0.6f, sh,  0.6f);
		glTexCoord2f(0,0); glVertex3f(-0.6f, sh,  0.6f);
	glEnd();
	glDepthMask(GL_TRUE);

	// Raise up if we're turning
	if(turning)
		glTranslatef(0, turning_raise * 0.85f, 0);


	// Negate the camera rotation
	glMultMatrixf(cam_neg_matrix);
//	glRotatef(45.0f, 0,1,0);
//	glRotatef(-30.0f, 1,0,0);

	// Draw the sprite
	if(burning)
		glColor3f(.5f,.5f,.5f);

	// Compute the texture coords according the animation frame and direction
	BIND_TEXTURE(enemy_anim[type]);
	int f = anim_frames[(int)anim];
	float textx = 0.25f * f;
	float texty = 0.25f * (3-dir);

	if(!dying) {
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(textx + 0.25f, texty + 0.25f); glVertex3f( size,  size,  size);
			glTexCoord2f(textx, texty + 0.25f); glVertex3f(-size,  size,  size);
			glTexCoord2f(textx + 0.25f, texty); glVertex3f( size, -size, -size);
			glTexCoord2f(textx, texty); glVertex3f(-size, -size, -size);
		glEnd();
	}
	else {
		// Draw the dying animation
		float z = size - (2*size*(1-die_anim));
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(textx + 0.25f, texty + (die_anim*0.25f)); glVertex3f( size,  z,  z);
			glTexCoord2f(textx, texty + (die_anim*0.25f)); glVertex3f(-size,  z,  z);
			glTexCoord2f(textx + 0.25f, texty); glVertex3f( size, -size, -size);
			glTexCoord2f(textx, texty); glVertex3f(-size, -size, -size);
		glEnd();

	}

	glPopMatrix();
}


// Clear the enemy
void ENEMY::clear() {
	x = y = 0;
	tx = ty = 0;
	offset = 0.0f;
	speed = 0.05f;
	dir = RAND(DIR_N, DIR_W);
	nextdir = dir;
	size = 0.85f;
	anim = 0.0f;
	type = 0;
	alive = false;
	dying = false;
	die_anim = 1.0f;
	path_pos = -1;
	chase = 0;
	burning = false;
	burn_time = 0;
	turning = 0;
	turning_counter = 0;
	turning_raise = 0.0f;
	kicked = false;
}


