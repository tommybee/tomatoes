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
#include "enemy.h"
#include "tilemap.h"
#include "bomb.h"
#include "particle.h"
#include "effects.h"
#include "font.h"
#include "teleport.h"
#include "special_power.h"
#include "soundmusic.h"
#include "comments.h"
#include "select_special.h"
#include "levels.h"
#include "helpers.h"


// This a dirty hack. I use this to modify the tilemap rendering to disable
// the depth writing for the particular block where the player is standing.
// Otherwise the player's feet would be cut off annoyingly. Since I'm modifying
// the tilemap, I can't use the display list when the player is over a block.
// This includes a speed penalty but it can't be helped. Of course I'm using
// the tilemap's display list when the player is not on a block.
int players_on_block_x[2] = { -1, -1 };
int players_on_block_y[2] = { -1, -1 };


// Players
PLAYER p1;
PLAYER p2;

// Player animations
GLuint player1_anim;
static int anim_frames[4] = { 0, 1, 0, 2 };

// Player sprite shadow
GLuint sprite_shadow;

// Player icons for two player mode
GLuint p_icons;
float p_icon_alpha[2] = { 0, 0 };


// Teleport particle from teleport.cpp
extern GLuint part_teleport;


// Moving style defines
#define MOV_RELATIVE		1
#define MOV_ABSOLUTE		2


#ifdef EDITOR
#define using_special_power			100
#define which_special_power			100
#endif



// Load player textures
void load_players() {
	// Load the animation
	player1_anim = load_png("player1.png", true, false, false);

	// Load the shadow
	sprite_shadow = load_png("shadow.png", true, false, true);

	// Load the icons
	p_icons = load_png("picons.png", true, false, false);
}


// Draw the player icons
void draw_player_icons() {
	if(!two_players)
		return;

	float offx = 0, offz = 0;
	float size = 0.5f;

	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	BIND_TEXTURE(p_icons);

	// For player #1
	if(p1.alive && p_icon_alpha[0]) {
		// Calculate the offset
		switch(p1.dir) {
			default:
			case DIR_N: offz = -p1.offset; break;
			case DIR_E: offx = p1.offset; break;
			case DIR_S: offz = p1.offset; break;
			case DIR_W: offx = -p1.offset; break;
		}

		// Translate to the position
		glPushMatrix();
		glTranslatef(p1.x + offx + 0.5f, p1.size - 0.20f, p1.y + offz + 0.5f);
		glTranslatef(0, 1.0f, 0);

		// Take the jumping position in to account
		#ifndef EDITOR
		if(p1.jumping) {
			VECT jpos = 0.0f;
			jpos = p1.jump_dir * p1.jump_pos * p1.jump_dist;
			glTranslatef(jpos.x, jpos.y, jpos.z);
		}
		#endif

		// If we're jumped over a block, raise the icon up
		if(map[p1.x][p1.y][1])
			glTranslatef(0, TILE_H, 0);

		// If we're jumping, translate up to the position
		#ifndef EDITOR
		if(p1.jumping) {
			float jy = p1.jump_height * SIN(180.0f * p1.jump_pos);

			// If we're jumping to a block, make sure we don't "sink" in it
			if(map[p1.jump_tx][p1.jump_ty][1] && jy < TILE_H && p1.jump_pos > 0.5f)
				jy = TILE_H;

			glTranslatef(0, jy, 0);
		}

		// If we're teleporting using the teleport special, raise ourselves up!
		if(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT)
			glTranslatef(0, TELEPORT_POWER_HEIGHT * sp_teleport.teleport_pos, 0);
		#endif

		// Negate the camera rotation
		glMultMatrixf(cam_neg_matrix);

		// Draw the sprite
		glColor4f(1,1,1, p_icon_alpha[0]);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.5f, 1); glVertex3f( size,  size,  size);
			glTexCoord2f(0, 1); glVertex3f(-size,  size,  size);
			glTexCoord2f(0.5f, 0); glVertex3f( size, -size, -size);
			glTexCoord2f(0, 0); glVertex3f(-size, -size, -size);
		glEnd();

		glPopMatrix();
	}


	// For player #2
	if(p2.alive && p_icon_alpha[1]) {
		offz = offx = 0;
		// Calculate the offset
		switch(p2.dir) {
			default:
			case DIR_N: offz = -p2.offset; break;
			case DIR_E: offx = p2.offset; break;
			case DIR_S: offz = p2.offset; break;
			case DIR_W: offx = -p2.offset; break;
		}

		// Translate to the position
		glPushMatrix();
		glTranslatef(p2.x + offx + 0.5f, p2.size - 0.20f, p2.y + offz + 0.5f);
		glTranslatef(0, 1.0f, 0);

		// If the players are sharing a block, make the P2 icon go a bit higher
		if(p1.x == p2.x && p1.y == p2.y)
			glTranslatef(0, 1.0f, 0);

		// Take the jumping position in to account
		#ifndef EDITOR
		if(p2.jumping) {
			VECT jpos = 0.0f;
			jpos = p2.jump_dir * p2.jump_pos * p2.jump_dist;
			glTranslatef(jpos.x, jpos.y, jpos.z);
		}
		#endif

		// If we're jumped over a block, raise the icon up
		if(map[p2.x][p2.y][1])
			glTranslatef(0, TILE_H, 0);

		// If we're jumping, translate up to the position
		#ifndef EDITOR
		if(p2.jumping) {
			float jy = p2.jump_height * SIN(180.0f * p2.jump_pos);

			// If we're jumping to a block, make sure we don't "sink" in it
			if(map[p2.jump_tx][p2.jump_ty][1] && jy < TILE_H && p2.jump_pos > 0.5f)
				jy = TILE_H;

			glTranslatef(0, jy, 0);
		}

		// If we're teleporting using the teleport special, raise ourselves up!
		if(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT)
			glTranslatef(0, TELEPORT_POWER_HEIGHT * sp_teleport.teleport_pos, 0);
		#endif

		// Negate the camera rotation
		glMultMatrixf(cam_neg_matrix);

		// Draw the sprite
		glColor4f(1,1,1, p_icon_alpha[1]);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1, 1); glVertex3f( size,  size,  size);
			glTexCoord2f(0.5f, 1); glVertex3f(-size,  size,  size);
			glTexCoord2f(1, 0); glVertex3f( size, -size, -size);
			glTexCoord2f(0.5f, 0); glVertex3f(-size, -size, -size);
		glEnd();

		glPopMatrix();
	}


	glEnable(GL_DEPTH_TEST);
}


// Show the player icon for 'who'
void show_icon(int who) {
	p_icon_alpha[who] = 1.0f;
}


// Helper function which returns a random block around a
// non-block position
void get_random_block_at(int x, int y, int &bx, int &by) {
	int dx = RAND(-1,1);
	int dy = RAND(-1,1);
	int counter = 0;
	while((!map_solid(x+dx, y+dy) || (x+dx < 0 || y+dy < 0 || x+dx > MAP_W-1 || y+dy > MAP_H-1)
			|| (x+dx == x && y+dy == y)) && counter < 10000) {
		dx = RAND(-1,1);
		dy = RAND(-1,1);
	}

	bx = x + dx;
	by = y + dy;
}


// Helper function which returns a respawn position
void get_respawn_position(int x, int y, int &rx, int &ry) {

	int ox, oy;
	bool ok = false;
	while(!ok) {
		get_random_block_at(x, y, ox, oy);
		int count = 0;
		if(!map_solid(ox+1, oy))
			count++;
		if(!map_solid(ox-1, oy))
			count++;
		if(!map_solid(ox, oy+1))
			count++;
		if(!map_solid(ox, oy-1))
			count++;

		if(count)
			ok = true;
	}

	rx = ox;
	ry = oy;
}




// Move the player
void PLAYER::move() {
#ifndef EDITOR
	int who = (this == &p1) ? 1 : 2;
	int who2 = who-1; 	// Used for array indices

	// Reduce the icon alpha
	if(p_icon_alpha[who2]) {
		p_icon_alpha[who2] -= 0.005f;
		if(p_icon_alpha[who2] < 0.0f)
			p_icon_alpha[who2] = 0.0f;
	}

	// If we're dead, reduce the death counter and respawn
	if(!alive) {
		death_counter--;
		if(death_counter == 0) {
			// Respawn to a block
			int ox, oy;
			get_respawn_position((int)get_real_x(), (int)get_real_y(), ox, oy);

			int odir = dir;
			//clear();
			alive = true;
			x = ox;
			y = oy;
			dir = odir;
			nextdir = dir;
			tx = x;
			ty = y;
			walking = false;
			jumping = false;
			dying = false;
			offset = 0.0f;
			create_teleport_effect(x, y);
			show_icon(who2);

			// Play the appear sound
			play_sound(SND_APPEAR, false);

		}
		return;
	}

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
			VECT dir = 0.0f;
			float c1[4] = { 0.1f, 0.7f, 1, 1 };
			float c2[4] = { 0.1f, 0.7f, 1, 0 };
			add_particle(pos, dir, RAND(20,35), 0.1f, 0.4f, c1, c2, part_star);
		}

		if(die_anim < 0.0f) {
			die_anim = 0.0f;
			alive = false;

			// Explode the player bombs
			if(num_bombs > 0) {
				list<BOMB>::iterator b;
				for(b = bomblist.begin(); b != bomblist.end(); ++b)
					if((*b).owner == who && (*b).time > 1)
						(*b).time = 1;		// Makes the bomb explode on the next cycle
			}
		}

		return;
	}


	// Jumping stuff
	if(jumping) {
		jump_pos += jump_speed;
		if(jump_pos >= 1.0f) {
			jump_pos = 1.0f;

			// We're now on the target tile
			x = jump_tx;
			y = jump_ty;
			tx = x;
			ty = y;
			offset = 0.0f;
			jumping = false;
		}

		// Create some particles if we're teleporting
		if(in_teleport && jumping) {
			VECT pos(get_real_x(), 0.25f, get_real_y());
			pos += jump_dir * jump_pos * jump_dist;
			pos.y += jump_height * SIN(180.0f * jump_pos);

			VECT dir;
			for(int f=0; f<5; f++) {
				VECT ppos = pos + VECT(RANDF(-0.5f,0.5f),RANDF(-0.5f,0.5f),RANDF(-0.5f,0.5f));
				dir.x = dir.y = dir.z = 0.0f;
				float c1[4] = { 0.3, 0.7f, 1, 1 };
				float c2[4] = { 0, 0, 1, 0 };

				add_particle(ppos, dir, RAND(10,30), 0.1f, 0.3f, c1, c2, part_teleport);
			}
		}


		// This is a dirty hack. Read the comments from the beginning of this file.
		if(map[jump_tx][jump_ty][1] && jump_pos > 0.9f) {
			players_on_block_x[who2] = jump_tx;
			players_on_block_y[who2] = jump_ty;
		}

		return;
	}

	// This is a dirty hack. Read the comments from the beginning of this file.
	if(map[x][y][1]) {
		players_on_block_x[who2] = x;
		players_on_block_y[who2] = y;
		//return;
	}
	else {
		players_on_block_x[who2] = -1;
	}

	// Don't move if we're using the napalm or the teleport power
	if(using_special_power && (which_special_power == RED_POWER_NAPALM))
		return;
	if(using_special_power == who && (which_special_power == BLUE_POWER_TELEPORT))
		return;

	// Don't move if the level is finished
	if(level_pause)
		return;


	// Advance the animation
	anim += 0.20f;
	if((int)anim > 3)
		anim = 0.0f;

	// Advance the turning animation
	if(turning) {
		turning_counter++;
		if(turning_counter == 5) {
			dir = nextdir;
			nextdir = dir + 1;
			if(nextdir > DIR_W)
				nextdir = DIR_N;
		}
		else if(turning_counter == 10) {
			dir = nextdir;
			turning = false;
		}
	}


	if(!walking && ((config.moving_style[who2] == MOV_RELATIVE && !key[config.key_up[who2]]) || (config.moving_style[who2] == MOV_ABSOLUTE && !key[config.key_up[who2]] && !key[config.key_down[who2]] && !key[config.key_left[who2]] && !key[config.key_right[who2]])))
		anim = 0.0f;

	// Check if we're on a block
	bool on_block = false;
	if(map_solid(x,y))
		on_block = true;

	// Don't move if we're using the flower power (absolute)
	if(on_block && config.moving_style[who2] == MOV_ABSOLUTE && (p1.num_flower_bombs > 0 || p2.num_flower_bombs > 0))
		return;


	// Check for turning input
	if(key[config.key_left[who2]]) {
		if(config.moving_style[who2] == MOV_RELATIVE) {
			// Relative moving
			if(!turn_key_down[0] && !turning) {
				// Turn left
				nextdir = dir - 1;
				if(nextdir < DIR_N)
					nextdir = DIR_W;

				if(!walking)
					dir = nextdir;

				turn_key_down[0] = true;
			}
		}
		else if(config.moving_style[who2] == MOV_ABSOLUTE && !walking) {
			// Absolute moving
			dir = DIR_W;
			walking = true;
			offset = 0.0f;

			tx = x - 1;
			ty = y;

			// Check if the target is passable?
			if(map_solid(tx, ty)) {
				tx = x;
				ty = y;
				walking = false;
			}

			if(on_block) {
				// We're on a block, jump down from it
				jump(tx, ty, 2.0f, 0.05f);
				tx = x;
				ty = y;
				anim = 0;
				on_block = true;

				// Play the jumping sound
				if(jumping)
					play_sound(SND_JUMP, false);
			}
		}
	}
	else
		turn_key_down[0] = false;

	if(key[config.key_right[who2]]) {
		if(config.moving_style[who2] == MOV_RELATIVE) {
			// Relative moving
			if(!turn_key_down[1] && !turning) {
				// Turn right
				nextdir = dir + 1;
				if(nextdir > DIR_W)
					nextdir = DIR_N;

				if(!walking)
					dir = nextdir;

				turn_key_down[1] = true;
			}
		}
		else if(config.moving_style[who2] == MOV_ABSOLUTE && !walking) {
			// Absolute moving
			dir = DIR_E;
			walking = true;
			offset = 0.0f;

			tx = x + 1;
			ty = y;

			// Check if the target is passable?
			if(map_solid(tx, ty)) {
				tx = x;
				ty = y;
				walking = false;
			}

			if(on_block) {
				// We're on a block, jump down from it
				jump(tx, ty, 2.0f, 0.05f);
				tx = x;
				ty = y;
				anim = 0;
				on_block = true;

				// Play the jumping sound
				if(jumping)
					play_sound(SND_JUMP, false);
			}
		}
	}
	else
		turn_key_down[1] = false;

	// Check for 180 degree turning
	if(key[config.key_down[who2]]) {
		if(config.moving_style[who2] == MOV_RELATIVE) {
			// Relative moving
			if(!turn_key_down[2] && !turning && !walking && !key[config.key_up[who2]]) {
				nextdir = dir + 1;
				if(nextdir > DIR_W)
					nextdir = DIR_N;
				turning = true;
				turning_counter = 0;

				turn_key_down[2] = true;
			}
		}
		else if(config.moving_style[who2] == MOV_ABSOLUTE && !walking) {
			// Absolute moving
			dir = DIR_S;
			walking = true;
			offset = 0.0f;

			tx = x;
			ty = y + 1;

			// Check if the target is passable?
			if(map_solid(tx, ty)) {
				tx = x;
				ty = y;
				walking = false;
			}

			if(on_block) {
				// We're on a block, jump down from it
				jump(tx, ty, 2.0f, 0.05f);
				tx = x;
				ty = y;
				anim = 0;
				on_block = true;

				// Play the jumping sound
				if(jumping)
					play_sound(SND_JUMP, false);
			}
		}
	}
	else
		turn_key_down[2] = false;

	// Don't move if we're using the flower power (relative)
	if(on_block && config.moving_style[who2] == MOV_RELATIVE && (p1.num_flower_bombs > 0 || p2.num_flower_bombs > 0))
		return;

	// Check for walking input
	if(key[config.key_up[who2]] && !walking && !turning) {
		if(config.moving_style[who2] == MOV_RELATIVE) {
			// Relative moving
			walking = true;
			offset = 0.0f;

			dir = nextdir;

			switch(dir) {
				default:
				case DIR_N: tx = x; ty = y - 1; break;
				case DIR_E: tx = x + 1; ty = y; break;
				case DIR_S: tx = x; ty = y + 1; break;
				case DIR_W: tx = x - 1; ty = y; break;
			}

			// Check if the target is passable?
			if(map_solid(tx, ty)) {
				tx = x;
				ty = y;
				walking = false;
			}

			if(on_block) {
				// We're on a block, jump down from it
				jump(tx, ty, 2.0f, 0.05f);
				tx = x;
				ty = y;
				anim = 0;
				on_block = true;

				// Play the jumping sound
				if(jumping)
					play_sound(SND_JUMP, false);
			}
		}
		else {
			// Absolute moving
			dir = DIR_N;
			walking = true;
			offset = 0.0f;

			tx = x;
			ty = y - 1;

			// Check if the target is passable?
			if(map_solid(tx, ty)) {
				tx = x;
				ty = y;
				walking = false;
			}

			if(on_block) {
				// We're on a block, jump down from it
				jump(tx, ty, 2.0f, 0.05f);
				tx = x;
				ty = y;
				anim = 0;
				on_block = true;

				// Play the jumping sound
				if(jumping)
					play_sound(SND_JUMP, false);
			}
		}
	}


	// Move towards the target tile
	if(offset < 1.0f && (tx != x || ty != y)) {
		offset += 0.1f;

		// If we're reached the target tile, move again
		if(offset >= 1.0f) {
			x = tx;
			y = ty;
			offset = 0.0f;
			walking = false;

			in_teleport = 0;
		}
	}

	// Reload the weapons
	if(reload > 0)
		reload--;

	// Dropping bombs
	if(key[config.key_shoot[who2]] && reload == 0 && num_bombs < 3 && !on_block && !icon_menu.wait) {
		reload = 30;

		// Plant the bomb
		add_bomb(x, y, BTYP_NORMAL, who);
		num_bombs++;

		// Play the sound
		play_sound(SND_BOMB, false);
	}


	// Invoke the special powers
	if(key[config.key_special[who2]]) {
		open_icon_menu(who, on_block);
		show_icon(0);
		show_icon(1);
	}

#endif
}


// The player dies
void PLAYER::die() {
	if(jumping)
		return;

	if(dying)
		return;		// Hey, don't you die twice, man! ;)

	dying = true;
	die_anim = 1.0f;
	death_counter = 80;			// Just over one second

	// Play the sound
	static int last_sound = -1;
	int sound = last_sound;
	while(sound == last_sound)
		sound = RAND(SND_DIE1, SND_DIE6);
	play_sound(sound, false);
	last_sound = sound;

	// Reduce the special powers
	for(int f=0; f<NUM_ICONS; f++) {
		//icon_menu.count[f] = 0;
		if(icon_menu.count[f])
			icon_menu.count[f]--;
	}


	// Add a comment
	add_comment(COL_DEFAULT, "Respawning soon...");
}


// From bonus.cpp
extern int powers_from_bonuses;


// Pick a bonus up
void PLAYER::pick_bonus(BONUS *b) {
#ifndef EDITOR
	b->alive = false;
	b->effect();

	if(wisp.bonus == b)
		wisp.bonus = NULL;

	// Handle the bonuses
	bonus_counter[b->type]++;
	if(bonus_counter[b->type] >= powers_from_bonuses) {
		// Add a comment
		switch(next_bonus[b->type]) {
			default:
			case RED_POWER_TRAP:
				add_comment(COL_RED, "You have received a Trap."); break;
			case RED_POWER_WILDFIRE:
				add_comment(COL_RED, "You have received a Wild Fire."); break;
			case RED_POWER_NAPALM:
				add_comment(COL_RED, "You have received a Napalm Strike."); break;
			case GREEN_POWER_WISP:
				add_comment(COL_GREEN, "You have received a Will O' The Wisp."); break;
			case GREEN_POWER_POTATOMAN:
				add_comment(COL_GREEN, "You have received a Potato Man."); break;
			case GREEN_POWER_FLOWERPOWER:
				add_comment(COL_GREEN, "You have received a Flower Power."); break;
			case BLUE_POWER_TELEPORT:
				add_comment(COL_BLUE, "You have received a Teleport."); break;
			case BLUE_POWER_TURN:
				add_comment(COL_BLUE, "You have received a Turn."); break;
			case BLUE_POWER_LIGHTNING:
				add_comment(COL_BLUE, "You have received a Lightning Bolt."); break;
		}

		// Handle the special power
		icon_menu.count[next_bonus[b->type]]++;
		next_bonus[b->type]++;
		switch(b->type) {
			default:
			case BONUS_RED:
				if(next_bonus[BONUS_RED] > RED_POWER_NAPALM)
					next_bonus[BONUS_RED] = RED_POWER_TRAP;
				break;
			case BONUS_GREEN:
				if(next_bonus[BONUS_GREEN] > GREEN_POWER_FLOWERPOWER)
					next_bonus[BONUS_GREEN] = GREEN_POWER_WISP;
				break;
			case BONUS_BLUE:
				if(next_bonus[BONUS_BLUE] > BLUE_POWER_LIGHTNING)
					next_bonus[BONUS_BLUE] = BLUE_POWER_TELEPORT;
				break;
		}

		bonus_counter[b->type] = 0;
	}

#endif
}


// Get current x with offset
float PLAYER::get_real_x() {
	// Calculate the offset
	float offx = 0;
	if(dir == DIR_E)
		offx = offset;
	else if(dir == DIR_W)
		offx = -offset;

	return (float)x + offx + 0.5f;
}


// Get current y with offset
float PLAYER::get_real_y() {
	// Calculate the offset
	float offy = 0;
	if(dir == DIR_N)
		offy = -offset;
	else if(dir == DIR_S)
		offy = offset;

	return (float)y + offy + 0.5f;
}


// Teleport power instance from special_power.cpp
#ifndef EDITOR
extern SP_TELEPORT sp_teleport;
#endif

// Draw the player
void PLAYER::draw() {
	if(!alive)
		return;

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

	// Take the jumping position in to account
	int jpox = -1;
	int jpoy = -1;
	float shadow_offset = 0.0f;
#ifndef EDITOR
	if(jumping) {
		VECT jpos = 0.0f;
		jpos = jump_dir * jump_pos * jump_dist;
		glTranslatef(jpos.x, jpos.y, jpos.z);

		// If we're over a block, raise the shadow a bit higher
		jpox = (int)(x + offx + 0.5f + jpos.x);
		jpoy = (int)(y + offz + 0.5f + jpos.z);
		if(map[jpox][jpoy][1])
			shadow_offset = TILE_H + 0.01f;
		else if(!map[jpox][jpoy][0])
			// Translate a waaaay down, so that the shadow can't be seen
			shadow_offset = -100.0f;
	}
#endif


	// If we're jumped over a block, raise the player up
	if(map[x][y][1]) {
		glTranslatef(0, TILE_H, 0);
		shadow_offset = 0.01f;
	}

	// Draw the shadow
	glDepthMask(GL_FALSE);
	glColor3f(1,1,1);
	BIND_TEXTURE(sprite_shadow);
	float sh = -(size - 0.21f);
	glTranslatef(0, shadow_offset, 0);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 0.6f, sh, -0.6f);
		glTexCoord2f(0,1); glVertex3f(-0.6f, sh, -0.6f);
		glTexCoord2f(1,0); glVertex3f( 0.6f, sh,  0.6f);
		glTexCoord2f(0,0); glVertex3f(-0.6f, sh,  0.6f);
	glEnd();
	glTranslatef(0, -shadow_offset, 0);
	glDepthMask(GL_TRUE);

	// If we're jumping, translate up to the position
#ifndef EDITOR
	if(jumping) {
		float jy = jump_height * SIN(180.0f * jump_pos);

		// If we're jumping to a block, make sure we don't "sink" in it
		if(map[jump_tx][jump_ty][1] && jy < TILE_H && jump_pos > 0.5f)
			jy = TILE_H;

		glTranslatef(0, jy, 0);
	}

	// If we're teleporting using the teleport special, raise ourselves up!
	int who = (this == &p1) ? 1 : 2;
	if(using_special_power == who && which_special_power == BLUE_POWER_TELEPORT)
		glTranslatef(0, TELEPORT_POWER_HEIGHT * sp_teleport.teleport_pos, 0);
#endif

	// Negate the camera rotation
	glMultMatrixf(cam_neg_matrix);
//	glRotatef(45.0f, 0,1,0);
//	glRotatef(-30.0f, 1,0,0);

	// Compute the texture coords according the animation frame and direction
	BIND_TEXTURE(player1_anim);
	int f = anim_frames[(int)anim];
	float textx = 0.25f * f;
	float texty = 0.25f * (3-dir);

	// Draw the sprite
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


// Clear the player
void PLAYER::clear() {
	x = y = 0;
	tx = ty = 0;
	offset = 0.0f;
	dir = DIR_E;
	nextdir = dir;
	size = 0.75f;
	anim = 0.0f;
	walking = false;
	turning = false;
	turning_counter = 0;
	reload = 0;
	num_bombs = 0;
	num_flower_bombs = 0;
	alive = true;
	death_counter = 0;
	turn_key_down[0] = turn_key_down[1] = turn_key_down[2] = false;
	in_teleport = 0;

	jumping = false;
	jump_pos = jump_dist = jump_height = jump_speed = 0.0f;
	jump_tx = jump_ty = 0;
	jump_dir = 0.0f;

	dying = false;
	die_anim = 0.0f;
}


// Jump
void PLAYER::jump(int tx, int ty, float height, float speed) {
	if(jumping)
		return;

	if(tx == (int)get_real_x() && ty == (int)get_real_y())
		return;

	jumping = true;

	// Direction vector
	float dx = ((float)tx + 0.5f) - get_real_x();
	float dy = ((float)ty + 0.5f) - get_real_y();
	jump_dir.x = dx;
	jump_dir.z = dy;
	jump_dir.y = 0;
	normalize(jump_dir);

	// Compute the distance
	jump_dist = sqrtf(dx*dx + dy*dy);

	jump_pos = 0.0f;
	jump_height = height;
	jump_speed = speed;

	// Save the target tile
	jump_tx = tx;
	jump_ty = ty;

	walking = false;
	anim = 0;
}


