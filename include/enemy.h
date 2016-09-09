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

#ifndef ENEMY_H
#define ENEMY_H

#include <list>
using namespace std;

#include "pathfinder.h"
#include "player.h"

// Number of enemies
#define ENEMY_AMOUNT		5


// Enemy animations
extern GLuint enemy1_anim;


// Enemy class
class ENEMY {
public:
	int x, y;						// Tile position
	int tx, ty;						// Target tile position
	float offset;					// Current offset
	float speed;					// Moving speed
	int dir;						// Direction
	int nextdir;					// Next direction

	int turning;					// Turning? (0 == false, 1 == raising up, 2 == turning, 3 == going down)
	float turning_raise;			// Raised position when turning
	int turning_counter;			// Counter for the turning animation

	float size;						// Sprite size
	float anim;						// Animation counter
	int type;						// Enemy type
	bool alive;						// Is the enemy alive?

	bool dying;						// Is the dying animation on?
	float die_anim;					// Die animation counter

	int chase;						// Are we chasing the players?

	bool burning;					// Are we burning?
	int burn_time;					// Burning time

	bool kicked;					// Has the potatoman kicked us?

	PATHFINDER pf;					// Path finder
	int path_pos;					// Current position along the path


	// Functions
	void clear();					// Clear the enemy
	void move();					// Move the enemy
	void draw();					// Draw the enemy
	void die();						// Kill the enemy
	void look_player();				// Look for the player and chase him
	float get_real_x();				// Get current x with offset
	float get_real_y();				// Get current y with offset
};

// Enemy list
extern list<ENEMY> enemylist;


void load_enemies();
void clear_enemies();
void draw_enemies();
void move_enemies();

#endif

