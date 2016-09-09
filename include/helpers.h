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

#ifndef HELPERS_H
#define HELPERS_H

#include "vect.h"
#include "bonus.h"
#include "pathfinder.h"


//////////////////////////////////////////////////
// WISP STUFF
//////////////////////////////////////////////////


// Wisp texture
extern GLuint wisp_tex;


// Wisp class
class WISP {
public:
	VECT pos;								// Position
	VECT target;							// Target position
	VECT dir;								// Direction vector
	BONUS *bonus;							// Target bonus
	int owner;								// Wisp owner (1 == p1, 2 == p2)

	bool alive;								// Is the wisp alive?
	int counter;							// Time counter
	float glow_anim;						// Glow animation counter

	// Functions
	void clear();							// Clear the wisp
	void draw();							// Draw the wisp
	void move();							// Move the wisp
};


void create_wisp();
void load_wisps();
void move_wisps();
void draw_wisps();




//////////////////////////////////////////////////
// POTATO MAN STUFF
//////////////////////////////////////////////////


// Potato man texture
extern GLuint potatoman_tex;

class ENEMY;


// Potato man class
class POTATOMAN {
public:
	int x, y;								// Tile position
	int tx, ty;								// Target tile position
	float offset;							// Current offset
	float speed;							// Moving speed
	int dir;								// Direction
	int nextdir;							// Next direction

	float anim;								// Animation counter
	bool alive;								// Is the potato man alive?

	bool chase;								// Chasing enemies?
	int counter;							// Time counter

	PATHFINDER pf;							// Path finder
	int path_pos;							// Current position along the path

	int raise_dir;							// Are we going up or down?
	float raise_pos;						// Raising position

	// Functions
	void clear();							// Clear the potato man
	void draw();							// Draw the potato man
	void move();							// Move the potato man
	void look_enemy();						// Look for enemies and chase them
	float get_real_x();						// Get current x with offset
	float get_real_y();						// Get current y with offset
	bool collide_with(ENEMY *e);			// Does the potato man collide with the enemy
	void kick(ENEMY *e);					// Kick the enemy
};


// The helpers
extern WISP wisp;
extern POTATOMAN potatoman;


void load_potatoman();
void move_potatomen();
void draw_potatomen();
void create_potatoman();

#endif
