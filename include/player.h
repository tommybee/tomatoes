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

#ifndef PLAYER_H
#define PLAYER_H

#include "init.h"
#include "vect.h"
#include "bonus.h"


// Direction defines
#define DIR_N		0
#define DIR_E		1
#define DIR_S		2
#define DIR_W		3

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_STAY	2


// Player class
class PLAYER {
public:
	int x, y;					// Tile position
	int tx, ty;					// Target tile position
	float offset;				// Current offset
	int dir;					// Direction
	int nextdir;				// Next direction

	float size;					// Sprite size
	float anim;					// Animation counter
	bool walking;				// Walking?
	bool turning;				// Turning?
	int turning_counter;		// Counter for the turning animation
	bool turn_key_down[3];		// Turning keys pressed down?

	bool alive;					// Is the player alive?
	int death_counter;			// Counts a few seconds before respawning
	int reload;					// Reloading counter
	int num_bombs;				// Number of active bombs (including flower bombs)
	int num_flower_bombs;		// Number of active flower bombs

	bool dying;					// Is the dying animation on?
	float die_anim;				// Die animation counter

	int in_teleport;			// Is the player teleporting?

	bool jumping;				// Is the player jumping?
	float jump_pos;	   			// Position in the jump arc (between 0 and 1)
	float jump_dist;			// Total jump distance
	float jump_height;			// Maximum jump height
	float jump_speed;			// Jump speed (less than 1)
	int jump_tx, jump_ty;		// Jump target tile
	VECT jump_dir;				// Jump direction vector (in 2D)

	// Functions
	void clear();				// Clear the player
	void move();				// Move the player
	void draw();				// Draw the player
	bool check_collisions();	// Check the enemy collisions
	float get_real_x();			// Get current x with offset
	float get_real_y();			// Get current y with offset
	void pick_bonus(BONUS *b);	// Pick a bonus up
	void die();					// The player dies

	// Jump
	void jump(int tx, int ty, float height, float speed);
};

// Players
extern PLAYER p1;
extern PLAYER p2;

void show_icon(int who);
void load_players();
void draw_player_icons();


#endif


