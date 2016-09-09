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
#include "texture.h"
#include "init.h"
#include "player.h"
#include "mymath.h"
#include "enemy.h"
#include "tilemap.h"
#include "particle.h"
#include "helpers.h"
#include "soundmusic.h"
#include "levels.h"


// The helpers
WISP wisp;
POTATOMAN potatoman;


//////////////////////////////////////////////////
// WISP STUFF
//////////////////////////////////////////////////


// The height where the wisp flies
#define WISP_HEIGHT			9.0f


// Wisp flying speed
#define WISP_SPEED			0.01f;


// Wisp texture
GLuint wisp_tex;


// Wisp time
const int wisp_counter = 1800;



// Create a wisp
void create_wisp() {
	wisp.clear();
	wisp.pos.x = RANDF(0,MAP_W+1);
	wisp.pos.y = 15.0f;
	wisp.pos.z = RANDF(0,MAP_H+1);
	wisp.owner = 1; 		// The owner is unused since the bonuses are common
	wisp.alive = true;
}


// Load the wisps
void load_wisps() {
	wisp_tex = load_jpg("wisp.jpg", false, false, true);
}


// Move the wisps
void move_wisps() {
	wisp.move();
}


// Draw the wisps
void draw_wisps() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	wisp.draw();

	glDepthMask(GL_TRUE);
}



// Move the wisp
void WISP::move() {
	if(!alive)
		return;

	// Don't move if the level is finished
	if(level_pause)
		return;

	// Advance the time
	counter++;
	if(counter > wisp_counter) {
		alive = false;
		// Create a particle explosion
		for(int f=0; f<RAND(10,20); f++) {
			VECT ppos(pos.x, pos.y, pos.z);
			VECT pdir(RANDF(-0.1f,0.1f),RANDF(-0.1f,0.1f),RANDF(-0.1f,0.1f));
			float c1[4] = { 0.1f, 1, 0.1f, 1 };
			float c2[4] = { 0.1f, 1, 0.1f, 0 };
			add_particle(ppos, pdir, RAND(30,60), RANDF(0.1f,0.2f), 0, c1, c2, part_glow);
		}

		play_sound(SND_WISP, false);
	}

	// Animate the glow
	glow_anim = add_angle(glow_anim, 6.0f);

	// Create a particle trail
	VECT ppos(pos.x, pos.y, pos.z);
	VECT pdir = 0.0f;
	float c1[4] = { 0.1f, 1, 0.1f, 1 };
	float c2[4] = { 0.1f, 1, 0.1f, 0 };
	float size = RANDF(0.05f, 0.15f);
	add_particle(ppos, pdir, RAND(10,30), size, size, c1, c2, part_glow);


	// If there are no targets, fly to the center of the screen
	if(!bonus) {
		// Target vector
		if(bonuslist.size() == 0) {
			target.x = MAP_W * 0.5f;
			target.y = WISP_HEIGHT;
			target.z = MAP_H * 0.5f;
		}
		else {
			// Choose a target
			bonus = &(*(bonuslist.begin()));
			target.x = bonus->x + 0.5f;
			target.z = bonus->y + 0.5f;
			target.y = 0.5f;
		}
	}

	// Head towards the target
	VECT tdir = target - pos;
	if(!is_zero_vector(tdir))
		normalize(tdir);
	float dx = pos.x - target.x;
	float dy = pos.y - target.y;
	float dz = pos.z - target.z;
	float dist = dx*dx + dy*dy + dz*dz;
	if(dist > 27 || !bonus) {
		dir += tdir * WISP_SPEED;
	}
	else {
		dir += tdir * 0.04f;
	}

	// Add some randomness
	if(RAND(0,100) > 95) {
		dir += VECT(RANDF(-0.01f,0.01f),RANDF(-0.01f,0.01f),RANDF(-0.01f,0.01f));
	}

	// Restrict the speed
	if(vector_length(dir) > 0.2f)
		set_vector_length(dir, 0.2f);

	// Move
	pos += dir;

	// If we're below the ground, fix that
	if(pos.y < 0.6f) {
		pos.y = 0.6f;
		dir.y *= -1.5f;
	}

	// Check the distance to the target
	if(bonus) {
		if(dist <= 1) {
			// Player gets the bonus
			// The bonuses really are common, so we just let p1 get it here.
			p1.pick_bonus(bonus);
		}
	}
}


// Draw the wisp
void WISP::draw() {
	if(!alive)
		return;

	// Translate to the position
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);


	// Negate the camera rotation
	glMultMatrixf(cam_neg_matrix);
//	glRotatef(45.0f, 0,1,0);
//	glRotatef(-30.0f, 1,0,0);


	// Draw the sprite
	glColor3f(1,1,1);
	BIND_TEXTURE(wisp_tex);
	float size = 0.2f;
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( size,  size,  size);
		glTexCoord2f(0,1); glVertex3f(-size,  size,  size);
		glTexCoord2f(1,0); glVertex3f( size, -size, -size);
		glTexCoord2f(0,0); glVertex3f(-size, -size, -size);
	glEnd();


	// Draw the glow
	BIND_TEXTURE(part_glow);
	glColor4f(0.1f,1,0.1f, 0.5f);
	size = 0.75f + (SIN(glow_anim) * 0.2f);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( size,  size,  size);
		glTexCoord2f(0,1); glVertex3f(-size,  size,  size);
		glTexCoord2f(1,0); glVertex3f( size, -size, -size);
		glTexCoord2f(0,0); glVertex3f(-size, -size, -size);
	glEnd();

	glPopMatrix();
}


// Clear the wisp
void WISP::clear() {
	pos = dir = target = 0.0f;
	bonus = NULL;
	owner = 0;
	alive = false;
	glow_anim = RANDF(0,359);
	counter = 0;
}


//////////////////////////////////////////////////
// POTATO MAN STUFF
//////////////////////////////////////////////////

// How far up does the potato man raise?
#define POTATO_HEIGHT		15.0f


// Potato man texture
GLuint potatoman_tex;
static int anim_frames[4] = { 0, 1, 0, 2 };

// Shadow texture from player.cpp
extern GLuint sprite_shadow;

// Function get_dir() from enemy.cpp
int get_dir(int dx, int dy);

// Potato man time
const int potato_counter = 900;


// Create a potato man
void create_potatoman() {
	potatoman.clear();
	int px = RAND(0, MAP_W-1);
	int py = RAND(0, MAP_H-1);
	while(map_solid(px, py)) {
		px = RAND(0, MAP_W-1);
		py = RAND(0, MAP_H-1);
	}
	potatoman.x = px;
	potatoman.y = py;
	potatoman.tx = potatoman.x;
	potatoman.ty = potatoman.y;
	potatoman.alive = true;
}


// Load the potato man
void load_potatoman() {
	potatoman_tex = load_png("potatoman.png", true, false, false);
}


// Draw the potato men
void draw_potatomen() {
	potatoman.draw();
}


// Move the potato men
void move_potatomen() {
	potatoman.move();
}


// Does the potato man collide with the enemy?
bool POTATOMAN::collide_with(ENEMY *e) {
	if(!alive)
		return false;

	if(raise_pos != 0.0f)
		return false;

	float dx = get_real_x() - e->get_real_x();
	float dy = get_real_y() - e->get_real_y();
	if(dx*dx + dy*dy <= 0.9f)
		return true;

	return false;
}


// Look for enemies and chase them
void POTATOMAN::look_enemy() {
	if(enemylist.size() == 0 || chase)
		return;

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

		// Check for the enemies
		list<ENEMY>::iterator i;
		for(i = enemylist.begin(); i != enemylist.end(); ++i) {
			if((int)(*i).get_real_x() == xx && (int)(*i).get_real_y() == yy && !(*i).dying) {
				// Begin the chase
				chase = true;
				speed += 0.03f;
				break;
			}

		if(chase)
			break;
		}
	}
}


// Kick the enemy
void POTATOMAN::kick(ENEMY *e) {
	e->kicked = true;
	e->chase = 0;
	e->speed = 0.2f;

	// Compute the right direction
	int pdir = dir + 2;
	if(pdir > DIR_W)
		pdir -= 4;

	if(e->dir == pdir || e->dir == dir)
		e->dir = dir;
	else {
		e->dir += 2;
		if(e->dir > DIR_W)
			e->dir -= 4;
	}

	e->nextdir = e->dir;
	e->offset = 0.0f;
	e->tx = e->x; e->ty = e->y;
	play_sound(SND_KICK, false);
}


// Move the potato man
void POTATOMAN::move() {
	if(!alive)
		return;

	// Don't move if the level is finished
	if(level_pause)
		return;

	// Create particles
	if(raise_pos != 0.0f) {
		VECT pos(get_real_x(), 0.25f, get_real_y());
		pos.y += POTATO_HEIGHT * raise_pos;

		VECT dir;
		for(int f=0; f<5; f++) {
			VECT ppos = pos + VECT(RANDF(-0.5f,0.5f),RANDF(-0.5f,0.5f),RANDF(-0.5f,0.5f));
			dir.x = dir.y = dir.z = 0.0f;
			float c1[4] = { 0.3f, 1, 0.3f, 1 };
			float c2[4] = { 1, 1, 0.3f, 0.1f };

			add_particle(ppos, dir, RAND(10,30), 0.3f, 0.1f, c1, c2, part_star);
		}
	}


	// Go up or down
	if(raise_dir == DIR_DOWN) {
		raise_pos -= 0.015f;
		if(raise_pos <= 0.0f) {
			raise_pos = 0.0f;
			raise_dir = -1;
		}

		return;
	}
	else if(raise_dir == DIR_UP) {
		raise_pos += 0.015f;
		if(raise_pos >= 1.0f) {
			raise_pos = 1.0f;
			alive = false;
		}

		return;
	}


	// Advance the time
	counter++;
	if(counter > potato_counter) {
		// Go up
		raise_dir = DIR_UP;
		raise_pos = 0.0f;
		anim = 0.0f;

		// Play the sound
		play_sound(SND_POTATOMAN2, false);
		return;
	}


	// Advance the animation
	if(!chase)
		anim += 0.20f;
	else
		anim += 0.30f;
	if((int)anim > 3)
		anim = 0.0f;


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
		look_enemy();
	}

	// Move one step
	if(tx == x && ty == y && path_pos > -1) {
		offset = 0.0f;

		// Follow the path if we're not chasing
		if(chase == false) {
			path_pos++;
			tx = pf.path[path_pos].x;
			ty = pf.path[path_pos].y;
			dir = get_dir(tx - x, ty - y);
			look_enemy();
		}
		else {
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
				chase = false;
				speed = 0.1f;
			}
		}

	}

	// Move towards the target tile
	if(offset < 1.0f && (tx != x || ty != y) && path_pos > -1) {
		offset += speed;

		// If we've reached the target tile, move again
		if(offset >= 1.0f) {
			x = tx;
			y = ty;
			offset = 0.0f;

			// If this is the final destination, stay put and choose a new path
			// on the next cycle
			if(x == pf.dx && y == pf.dy && !chase)
				path_pos = -1;
		}
	}

}


// Draw the potato man
void POTATOMAN::draw() {
	if(!alive)
		return;

	// Sprite size
	const float size = 0.85f;

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

	// Translate up
	glTranslatef(0, POTATO_HEIGHT * raise_pos, 0);


	// Negate the camera rotation
	glMultMatrixf(cam_neg_matrix);
//	glRotatef(45.0f, 0,1,0);
//	glRotatef(-30.0f, 1,0,0);

	// Draw the sprite

	// Compute the texture coords according the animation frame and direction
	BIND_TEXTURE(potatoman_tex);
	int f = anim_frames[(int)anim];
	float textx = 0.25f * f;
	float texty = 0.25f * (3-dir);

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(textx + 0.25f, texty + 0.25f); glVertex3f( size,  size,  size);
		glTexCoord2f(textx, texty + 0.25f); glVertex3f(-size,  size,  size);
		glTexCoord2f(textx + 0.25f, texty); glVertex3f( size, -size, -size);
		glTexCoord2f(textx, texty); glVertex3f(-size, -size, -size);
	glEnd();

	glPopMatrix();
}


// Get current x with offset
float POTATOMAN::get_real_x() {
	// Calculate the offset
	float offx = 0;
	if(dir == DIR_E)
		offx = offset;
	else if(dir == DIR_W)
		offx = -offset;

	return (float)x + offx + 0.5f;
}


// Get current y with offset
float POTATOMAN::get_real_y() {
	// Calculate the offset
	float offy = 0;
	if(dir == DIR_N)
		offy = -offset;
	else if(dir == DIR_S)
		offy = offset;

	return (float)y + offy + 0.5f;
}


// Clear the potato man
void POTATOMAN::clear() {
	x = y = tx = ty = 0;
	offset = 0.0f;
	speed = 0.1f;
	dir = RAND(DIR_N, DIR_W);
	nextdir = dir;
	anim = 0.0f;
	alive = false;
	path_pos = -1;
	chase = false;
	counter = 0;
	raise_dir = DIR_DOWN;
	raise_pos = 1.0f;
}

