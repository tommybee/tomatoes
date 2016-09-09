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

#ifndef TRAP_H
#define TRAP_H

#include <list>
using namespace std;


// Trap texture
extern GLuint trap_texture;


// Trap class
class TRAP {
public:
	int x, y;							// Position
	int counter;						// Trap counter
	int owner;							// Trap owner (1 == p1, 2 == p2)
	float anim;							// Animation counter
	bool alive;							// Does the trap exist?

	// Functions
	void clear();						// Clear the trap
	void draw();						// Draw the trap
	void move();						// Animate the trap
};

// Trap list
extern list<TRAP> traplist;

void load_traps();
void move_traps();
void draw_traps();
void clear_traps();
void add_trap(int x, int y, int owner);

#endif
