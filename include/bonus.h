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

#ifndef BONUS_H
#define BONUS_H

#include <list>
using namespace std;

// Bonus types
#define BONUS_RED			0
#define BONUS_GREEN			1
#define BONUS_BLUE			2

// Number of bonus types
#define NUM_BONUS			3


// Next bonus to come of each color
extern int next_bonus[NUM_BONUS];

// Bonus counters for each color
extern int bonus_counter[NUM_BONUS];


// Bonus sprite texture
extern GLuint bonus_tex;


// Bonus class
class BONUS {
public:
	int x, y;					// Position
	float z;					// Z position

	int type;					// Bonus type
	float size;					// Sprite size
	float anim_angle;			// Angle for animations
	bool alive;					// Is the bonus alive? (i.e. active)

	// Functions
	void clear();				// Clear the bonus
	void move();				// Move the bonus
	void draw();				// Draw the bonus
	void effect();				// Create the particle effect
};

// Bonus list
extern list<BONUS> bonuslist;


void add_bonus(int x, int y, int type, bool rain = false);

void load_bonus();
void move_bonus();
void draw_bonus();
void clear_bonus();

void create_bonus_rain();

#endif

