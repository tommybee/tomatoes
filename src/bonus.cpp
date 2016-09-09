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
#include "init.h"
#include "player.h"
#include "mymath.h"
#include "bonus.h"
#include "particle.h"
#include "game.h"
#include "special_power.h"
#include "soundmusic.h"
#include "comments.h"
#include "tilemap.h"
#include "levels.h"


// Bonus sprite texture
GLuint bonus_tex;
GLuint bonus_particle;

// Shadow texture from player.cpp
extern GLuint sprite_shadow;


// How many bonuses you have to collect in order to get
// a special power
int powers_from_bonuses = 2;


// Next bonus to come of each color
int next_bonus[NUM_BONUS] = { 0, };

// Bonus counters for each color
int bonus_counter[NUM_BONUS] = { 0, };

// This makes sure that the enemies don't teleport after you have
// killed 5 of them simultaneously, until you collect all the diamons
int killed_5_diamonds = 0;
Uint32 killed_5_time = 0;


// Bonus list
list<BONUS> bonuslist;



// Add a bonus
void add_bonus(int x, int y, int type, bool rain) {

	BONUS b;
	b.clear();
	b.x = x;
	b.y = y;
	if(rain)
		b.z = RANDF(15,18);

	b.type = type;
	b.alive = true;

	bonuslist.push_back(b);
}


// Load bonus textures
void load_bonus() {
	bonus_tex = load_png("bonus.png", true, false, false);
	bonus_particle = load_jpg("bonus_part.jpg", false, false, true);
}


// From enemy.cpp
extern int enemy_creation_counter;


// Create the "bonus rain"
void create_bonus_rain() {
	for(int f=0; f<10; f++) {
		// Choose a random location
		int bx = RAND(0, MAP_W-1);
		int by = RAND(0, MAP_H-1);
		while(map_solid(bx, by)) {
			bx = RAND(0, MAP_W-1);
			by = RAND(0, MAP_H-1);
		}
		add_bonus(bx, by, RAND(BONUS_RED, BONUS_BLUE), true);
	}

	enemy_creation_counter = 2 * 90;

	killed_5_diamonds = bonuslist.size();
	killed_5_time = SDL_GetTicks();
}


// Move the bonuses
void move_bonus() {
	if(bonuslist.size() == 0)
		return;

	bool bonuses_picked_up = false;

	list<BONUS>::iterator i;
	for(i = bonuslist.begin(); i != bonuslist.end(); ++i) {
		(*i).move();
		// Remove the dead bonuses
		if((*i).alive == false) {
			i = bonuslist.erase(i);
			bonuses_picked_up = true;
			if(killed_5_diamonds > 0) {
				killed_5_diamonds--;
				if(killed_5_diamonds == 0) {
					// Restore the time spent collecting the diamonds.
					level_time += (SDL_GetTicks() - killed_5_time);
				}
			}
		}
	}

	// Play the sound
	if(bonuses_picked_up) {
		static int last_sound = -1;
		int sound = last_sound;
		while(sound == last_sound)
			sound = RAND(SND_BONUS1, SND_BONUS3);
		play_sound(sound, false);
		last_sound = sound;
	}
}


// Create the particle effect
void BONUS::effect() {
	for(int f=0; f<RAND(40,60); f++) {
		VECT pos(x + 0.5f, 0.5f, y + 0.5f);
		VECT dir;
		dir.x = RANDF(-0.05f, 0.05f);
		dir.y = RANDF(0.05f, 0.09f);
		dir.z = RANDF(-0.05f, 0.05f);
		float c1[4] = { .1f, .1f, .1f, 1 };
		float c2[4] = { .1f, .1f, .1f, 0.5f };

		switch(type) {
			default:
			case BONUS_RED:
				c1[0] = c2[0] = 1; break;
			case BONUS_GREEN:
				c1[1] = c2[1] = 1; break;
			case BONUS_BLUE:
				c1[2] = c2[2] = 1; break;
		}
		add_particle(pos, dir, RAND(10,60), 0.2f, 0.04f, c1, c2, bonus_particle);
	}
}


// Draw the bonuses
void draw_bonus() {
	if(bonuslist.size() == 0)
		return;

	glColor3f(1,1,1);
	glDepthMask(GL_FALSE);

	list<BONUS>::iterator i;
	for(i = bonuslist.begin(); i != bonuslist.end(); ++i)
		(*i).draw();

	glDepthMask(GL_TRUE);
}


// Clear the bonuses
void clear_bonus() {
	bonuslist.clear();
	killed_5_diamonds = 0;
}


// Move the bonus
void BONUS::move() {
	// Animate
	anim_angle = add_angle(anim_angle, 5.0f);

	// Drop
	if(z > 0.0f) {
		z -= 0.15f;
		if(z < 0.0f)
			z = 0.0f;
	}

	// If the player comes close, he picks the bonus up
	if(z == 0.0f && (int)p1.get_real_x() == x && (int)p1.get_real_y() == y  && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT))
		p1.pick_bonus(this);

	if(two_players && z == 0.0f && (int)p2.get_real_x() == x && (int)p2.get_real_y() == y  && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT))
		p2.pick_bonus(this);
}


// Draw the bonus
void BONUS::draw() {
	// Translate to the position
	glPushMatrix();
	glTranslatef(x + 0.51f, 0.0f, y + 0.51f);

	// Draw the shadow
	glColor3f(1,1,1);
	BIND_TEXTURE(sprite_shadow);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 0.4f, 0, -0.4f);
		glTexCoord2f(0,1); glVertex3f(-0.4f, 0, -0.4f);
		glTexCoord2f(1,0); glVertex3f( 0.4f, 0,  0.4f);
		glTexCoord2f(0,0); glVertex3f(-0.4f, 0,  0.4f);
	glEnd();

	// Translate up
	glTranslatef(0, 0.5f + z, 0);
	//glTranslatef(0, 0.5f + (COS(anim_angle) * 0.15f), 0);


	// Negate the camera rotation
	glMultMatrixf(cam_neg_matrix);
//	glRotatef(45.0f, 0,1,0);
//	glRotatef(-30.0f, 1,0,0);

	// Draw the sprite
	float siz = size + COS(anim_angle) * 0.05f;		// Animate the size
	BIND_TEXTURE(bonus_tex);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.25f * type + 0.25f, 1);	glVertex3f( siz,  siz,  siz);
		glTexCoord2f(0.25f * type, 1);			glVertex3f(-siz,  siz,  siz);
		glTexCoord2f(0.25f * type + 0.25f, 0);	glVertex3f( siz, -siz, -siz);
		glTexCoord2f(0.25f * type, 0);			glVertex3f(-siz, -siz, -siz);
	glEnd();

	glPopMatrix();
}


// Clear the bonus
void BONUS::clear() {
	x = y = 0;
	z = 0.0f;
	anim_angle = 0;
	type = 0;
	size = 0.4f;
	alive = false;
}


