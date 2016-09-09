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
#include "game.h"
#include "texture.h"
#include "init.h"
#include "player.h"
#include "mymath.h"
#include "tilemap.h"
#include "enemy.h"
#include "bomb.h"
#include "particle.h"
#include "effects.h"
#include "helpers.h"
#include "special_power.h"
#include "soundmusic.h"
#include "comments.h"


// Bomb list
list<BOMB> bomblist;


// Bomb sprite textures
GLuint bomb_tex[NUM_BOMBS];

// Shadow texture from player.cpp
extern GLuint sprite_shadow;


// Add a bomb
void add_bomb(int x, int y, int type, int owner) {

	BOMB b;
	b.clear();
	b.x = x;
	b.y = y;

	b.owner = owner;
	b.type = type;
	b.time = 2 * 60;		// Two seconds
	b.alive = true;

	bomblist.push_back(b);
}


// Load bomb textures
void load_bombs() {
	bomb_tex[0] = load_png("bomb1.png", true, false, false);
	bomb_tex[1] = bomb_tex[0];
}


// Move the bombs
void move_bombs() {
	if(bomblist.size() == 0)
		return;

	list<BOMB>::iterator i;
	for(i = bomblist.begin(); i != bomblist.end(); ++i) {
		(*i).move();
		// Remove the dead bombs
		if((*i).alive == false) {
			i = bomblist.erase(i);
		}
	}
}


// Draw the bombs
void draw_bombs() {
	if(bomblist.size() == 0)
		return;

	glColor3f(1,1,1);

	list<BOMB>::iterator i;
	for(i = bomblist.begin(); i != bomblist.end(); ++i)
		if((*i).time > 0)
			(*i).draw();

}


// Clear the bombs
void clear_bombs() {
	bomblist.clear();
}


#define TRACED_P1			2
#define TRACED_P2			4


// Helper function which traces straight lines and checks the players
void trace_players(int sx, int sy, int dir, bool recurse, int &who) {

	// Begin tracing
	for(int pos = 1; pos < MAP_W; pos++) {

		// Compute the current position (xx,yy)
		int xx, yy;
		switch(dir) {
			default:
			case DIR_N: xx = sx; yy = sy - pos; break;
			case DIR_E: xx = sx + pos; yy = sy; break;
			case DIR_S: xx = sx; yy = sy + pos; break;
			case DIR_W: xx = sx - pos; yy = sy; break;
		}

		// Check if there's a wall
		if(map_solid(xx,yy))
			break;

		// Recurse if necessary
		if(recurse) {
			if(dir == DIR_N) {
				// Go to east or west
				if(!map_solid(xx + 1, yy))
					trace_players(xx, yy, DIR_E, false, who);
				else if(!map_solid(xx - 1, yy))
					trace_players(xx, yy, DIR_W, false, who);
			}
			else if(dir == DIR_E) {
				// Go to south or north
				if(!map_solid(xx, yy + 1))
					trace_players(xx, yy, DIR_S, false, who);
				else if(!map_solid(xx, yy - 1))
					trace_players(xx, yy, DIR_N, false, who);
			}
			else if(dir == DIR_S) {
				// Go to west or east
				if(!map_solid(xx - 1, yy))
					trace_players(xx, yy, DIR_W, false, who);
				else if(!map_solid(xx + 1, yy))
					trace_players(xx, yy, DIR_E, false, who);
			}
			else if(dir == DIR_W) {
				// Go to north or south
				if(!map_solid(xx, yy - 1))
					trace_players(xx, yy, DIR_N, false, who);
				else if(!map_solid(xx, yy + 1))
					trace_players(xx, yy, DIR_S, false, who);
			}

			if((who & TRACED_P1) && (two_players && (who & TRACED_P2)))
				return;

		}


		// Check also for the players
		if(p1.alive && (int)p1.get_real_x() == xx && (int)p1.get_real_y() == yy && !p1.jumping && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT))
			who |= TRACED_P1;

		if(two_players && p2.alive && (int)p2.get_real_x() == xx && (int)p2.get_real_y() == yy && !p2.jumping && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT))
			who |= TRACED_P2;

		if((who & TRACED_P1) && (two_players && (who & TRACED_P2)))
			return;
	}
}


// Helper function which checks if the player is on the line of fire of the flower power bomb
void trace_flower_power(int bx, int by, int &who) {

	// Check the bomb location first
	if((int)p1.get_real_x() == bx && (int)p1.get_real_y() == by && p1.alive && !p1.jumping && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT))
		who |= TRACED_P1;
	if(two_players && (int)p2.get_real_x() == bx && (int)p2.get_real_y() == by && p2.alive && !p2.jumping && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT))
		who |= TRACED_P2;

	if((who & TRACED_P1) && (two_players && (who & TRACED_P2)))
		return;

	// Sweep to each direction, and check the players
	for(int dir = DIR_N; dir <= DIR_W; dir++) {
		trace_players(bx, by, dir, true, who);
		if((who & TRACED_P1) && (two_players && (who & TRACED_P2)))
			break;
	}
}


// Helper function which traces straight lines and creates explosions
// Returns the number of enemies killed
int trace_explosion(int sx, int sy, int dir, int type, bool explosions, bool recurse) {
	int killed = 0;

	// Begin tracing
	for(int pos = 1; pos < MAP_W; pos++) {

		// Compute the current position (xx,yy)
		int xx, yy;
		switch(dir) {
			default:
			case DIR_N: xx = sx; yy = sy - pos; break;
			case DIR_E: xx = sx + pos; yy = sy; break;
			case DIR_S: xx = sx; yy = sy + pos; break;
			case DIR_W: xx = sx - pos; yy = sy; break;
		}

		// Check if there's a wall
		if(map_solid(xx,yy))
			break;

		// Recurse if necessary
		if(recurse) {
			if(dir == DIR_N) {
				// Go to east or west
				if(!map_solid(xx + 1, yy))
					trace_explosion(xx, yy, DIR_E, type, explosions, false);
				else if(!map_solid(xx - 1, yy))
					trace_explosion(xx, yy, DIR_W, type, explosions, false);
			}
			else if(dir == DIR_E) {
				// Go to south or north
				if(!map_solid(xx, yy + 1))
					trace_explosion(xx, yy, DIR_S, type, explosions, false);
				else if(!map_solid(xx, yy - 1))
					trace_explosion(xx, yy, DIR_N, type, explosions, false);
			}
			else if(dir == DIR_S) {
				// Go to west or east
				if(!map_solid(xx - 1, yy))
					trace_explosion(xx, yy, DIR_W, type, explosions, false);
				else if(!map_solid(xx + 1, yy))
					trace_explosion(xx, yy, DIR_E, type, explosions, false);
			}
			else if(dir == DIR_W) {
				// Go to north or south
				if(!map_solid(xx, yy - 1))
					trace_explosion(xx, yy, DIR_N, type, explosions, false);
				else if(!map_solid(xx, yy + 1))
					trace_explosion(xx, yy, DIR_S, type, explosions, false);
			}
		}


		// No wall, create explosion
		if(explosions)
			create_explosion(xx, yy, (type == BTYP_NORMAL) ? EXP_BOMB_NORMAL : EXP_BOMB_FLOWER);

		// Loop through the enemies and kill them
		list<ENEMY>::iterator e;
		for(e = enemylist.begin(); e != enemylist.end(); ++e) {
			if((int)((*e).get_real_x()) == xx && (int)((*e).get_real_y()) == yy && !(*e).dying) {
				(*e).die();
				killed++;
			}
		}

		// Check also for the players
		if(explosions && type == BTYP_NORMAL && p1.alive && (int)p1.get_real_x() == xx && (int)p1.get_real_y() == yy && !p1.jumping && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT))
			p1.die();

		if(explosions && two_players && type == BTYP_NORMAL && p2.alive && (int)p2.get_real_x() == xx && (int)p2.get_real_y() == yy && !p2.jumping && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT))
			p2.die();

/*		// Check the potato men
		if(explosions && potatoman.alive && (int)potatoman.get_real_x() == xx && (int)potatoman.get_real_y() == yy && potatoman.raise_pos == 0.0f) {
			// Raise up
			potatoman.raise_dir = DIR_UP;
			potatoman.raise_pos = 0.0f;
			potatoman.anim = 0.0f;

			// Play the sound
			play_sound(SND_POTATOMAN2, false);
		}
*/
	}

	return killed;
}


// Explode the bomb
// Returns the number of enemies killed
int BOMB::explode(bool explosions) {
	int num_killed = 0;

	// Reduce the owner's bomb count
	if(explosions) {
		if(owner == 1)
			p1.num_bombs--;
		else
			p2.num_bombs--;

		// Play the explosion sound
		play_sound(SND_EXPLO, true);
	}


	// Sweep to each direction, and wipe everything out
	for(int dir = DIR_N; dir <= DIR_W; dir++)
		num_killed += trace_explosion(x, y, dir, type, explosions, (type == BTYP_NORMAL) ? false : true);


	// Create explosion also to the bomb location
	if(explosions)
		create_explosion(x, y, (type == BTYP_NORMAL) ? EXP_BOMB_CENTER : EXP_BOMB_CENTER_FLOWER);

	// Check the enemies on here also
	// Loop through the enemies and kill them
	list<ENEMY>::iterator e;
	for(e = enemylist.begin(); e != enemylist.end(); ++e) {
		if((int)((*e).get_real_x()) == x && (int)((*e).get_real_y()) == y && !(*e).dying) {
			(*e).die();
			num_killed++;
		}
	}

	// Check also for the players
	if(explosions && type == BTYP_NORMAL && p1.alive && (int)p1.get_real_x() == x && (int)p1.get_real_y() == y && !p1.jumping && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT))
		p1.die();

	if(explosions && two_players && type == BTYP_NORMAL && p2.alive && (int)p2.get_real_x() == x && (int)p2.get_real_y() == y && !p2.jumping && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT))
		p2.die();

	// Check the potato men
/*	if(explosions && potatoman.alive && (int)potatoman.get_real_x() == x && (int)potatoman.get_real_y() == y && potatoman.raise_pos == 0.0f) {
		// Raise up
		potatoman.raise_dir = DIR_UP;
		potatoman.raise_pos = 0.0f;
		potatoman.anim = 0.0f;

		// Play the sound
		play_sound(SND_POTATOMAN2, false);
	}
*/
	return num_killed;
}


// Helper function from player.cpp
void get_random_block_at(int x, int y, int &bx, int &by);


// Move the bomb
void BOMB::move() {
	// Animate
	if(time > 0) {
		anim_angle = add_angle(anim_angle, 5.0f);

		// Fly
		if(z > 0.5f || z_speed != 0.0f) {
			z_speed -= 0.01f;
			z += z_speed;
			if(z < 0.5f) {
				z = 0.5f;
				z_speed = 0;

				// Play the flower power sound
				if(type == BTYP_FLOWER)
					play_sound(SND_FLOWERPOWER, false);
			}
			return;
		}
	}

	// Decrease the time
	time--;

	// Check if the players are affected by the flower power
	// If they are: jump
	if(type == BTYP_FLOWER && time <= 40 && time >= 0 && oldx[0] == -1 && oldx[1] == -1) {
		int who = 0;
		trace_flower_power(x, y, who);
		int jtx = 0, jty = 0;
		oldx[0] = oldx[1] = oldy[0] = oldy[1] = -1;
		if(who & TRACED_P1) {
			// P1 jumps
			oldx[0] = (int)p1.get_real_x();
			oldy[0] = (int)p1.get_real_y();
			//oldx[1] = oldy[1] = -1;
			get_random_block_at(oldx[0], oldy[0], jtx, jty);
			p1.jump(jtx, jty, 2.0f, 0.05f);

			// Play the jump sound
			play_sound(SND_JUMP, false);
		}
		if(who & TRACED_P2) {
			// P2 jumps
			oldx[1] = (int)p2.get_real_x();
			oldy[1] = (int)p2.get_real_y();
			//oldx[0] = oldy[0] = -1;
			get_random_block_at(oldx[1], oldy[1], jtx, jty);
			p2.jump(jtx, jty, 2.0f, 0.05f);

			// Play the jump sound
			play_sound(SND_JUMP, false);
		}
	}

	// Explode?
	if(time == 0) {
		killed = 0;
		killed = explode();
		return;
	}

	// After the initial explosion, we check the cells a few times to
	// make sure that the players/enemies touching the explosions flames
	// die as they should.
	if(time == -10)
		killed += explode(false);
	else if(time == -20)
		killed += explode(false);
	else if(time == -30)
		killed += explode(false);
	else if(time == -35)
		killed += explode(false);
	else if(time == -40) {
		killed += explode(false);
		if(type == BTYP_NORMAL) {
			alive = false;

			// Check if we have killed 5 or more enemies
			if(killed >= 5) {
				play_sound(SND_KILLED5, false);

				// Create the "bonus rain"
				create_bonus_rain();
				add_comment(COL_DEFAULT, "Five enemies with one bomb!");
			}
		}
	}

	// Jump the players back after the flower power explosions
	if(type == BTYP_FLOWER && time <= -60) {
		if(oldx[0] != -1) {
			// P1 jumps
			p1.jump(oldx[0], oldy[0], 2.0f, 0.05f);

			// Play the jump sound
			play_sound(SND_JUMP, false);
		}
		if(oldx[1] != -1) {
			// P2 jumps
			p2.jump(oldx[1], oldy[1], 2.0f, 0.05f);

			// Play the jump sound
			play_sound(SND_JUMP, false);
		}
		alive = false;

		// Decrease the active flower bomb amount
		if(owner == 1)
			p1.num_flower_bombs--;
		else
			p2.num_flower_bombs--;
	}

	// Create some particles
	if(time > 0 && RAND(0,100) > 30) {
		VECT pos(x + 0.6f, 0.85f + (SIN(anim_angle) * 0.2f), y + 0.6f);
		VECT dir;
		dir.x = RANDF(-0.01f, 0.01f);
		dir.y = RANDF(0.05f, 0.09f);
		dir.z = RANDF(-0.01f, 0.01f);
		if(type == BTYP_NORMAL) {
			float c1[4] = { 1, 0.7f, 0.3f, 1 };
			float c2[4] = { 1, 0, 0, 0 };
			add_particle(pos, dir, RAND(10,60), 0.05f, 0.2f, c1, c2, part_spark);
		}
		else if(type == BTYP_FLOWER) {
			float c1[4] = { 0.6f, 1, 0.3f, 1 };
			float c2[4] = { 0, 1, 0, 0 };
			add_particle(pos, dir, RAND(10,60), 0.05f, 0.2f, c1, c2, part_spark);
		}
	}
}


// Draw the bomb
void BOMB::draw() {
	// Translate to the position
	glPushMatrix();
	glTranslatef(x + 0.51f, 0.0f, y + 0.51f);

	// Draw the shadow
	glDepthMask(GL_FALSE);
	glColor3f(1,1,1);
	BIND_TEXTURE(sprite_shadow);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 0.5f, 0, -0.5f);
		glTexCoord2f(0,1); glVertex3f(-0.5f, 0, -0.5f);
		glTexCoord2f(1,0); glVertex3f( 0.5f, 0,  0.5f);
		glTexCoord2f(0,0); glVertex3f(-0.5f, 0,  0.5f);
	glEnd();
	glDepthMask(GL_TRUE);

	// Translate up and create waving motion
	glTranslatef(0, z + (SIN(anim_angle) * 0.2f), 0);


	// Negate the camera rotation
	glMultMatrixf(cam_neg_matrix);
//	glRotatef(45.0f, 0,1,0);
//	glRotatef(-30.0f, 1,0,0);

	// Draw the sprite
	BIND_TEXTURE(bomb_tex[type]);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( size,  size,  size);
		glTexCoord2f(0,1); glVertex3f(-size,  size,  size);
		glTexCoord2f(1,0); glVertex3f( size, -size, -size);
		glTexCoord2f(0,0); glVertex3f(-size, -size, -size);
	glEnd();

	glPopMatrix();
}


// Clear the bomb
void BOMB::clear() {
	x = y = 0;
	type = 0;
	size = 0.45f;
	owner = 0;
	anim_angle = RANDF(0,359);
	alive = false;
	time = 0;
	z = 0.5f;
	z_speed = 0.2f;
	oldx[0] = oldx[1] = oldy[0] = oldy[1] = -1;
	killed = 0;
}


