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

#ifndef BOMB_H
#define BOMB_H

#include <list>
using namespace std;

// Bomb types
#define BTYP_NORMAL			0
#define BTYP_FLOWER			1

// Number of bomb types
#define NUM_BOMBS			2


// Bomb sprite textures
extern GLuint bomb_tex[NUM_BOMBS];



// Bomb class
class BOMB {
public:
	int x, y;					// Position
	int oldx[2], oldy[2];		// Old player positions (used when jumping)
	float z;					// Z position
	float z_speed;				// Z speed (when throwing)
	int time;					// When time reaches zero, it explodes

	int type;					// Bomb type
	int owner;					// Bomb owner (1 or 2)
	float size;					// Sprite size
	float anim_angle;			// Angle for animations
	bool alive;					// Is the bomb alive? (i.e. active)

	int killed;					// How many enemies we killed with this bomb?


	// Functions
	void clear();				// Clear the bomb
	void move();				// Move the bomb
	void draw();				// Draw the bomb
	int explode(bool explosions = true);		// Explode the bomb
};

// Bomb list
extern list<BOMB> bomblist;


void add_bomb(int x, int y, int type, int owner);

void load_bombs();
void move_bombs();
void draw_bombs();
void clear_bombs();

#endif

