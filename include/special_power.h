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

#ifndef _SPECIAL_POWER
#define _SPECIAL_POWER

#include "enemy.h"

// Red special powers
#define RED_POWER_TRAP				0
#define RED_POWER_WILDFIRE			1
#define RED_POWER_NAPALM			2

// Green special powers
#define GREEN_POWER_WISP			3
#define GREEN_POWER_POTATOMAN		4
#define GREEN_POWER_FLOWERPOWER		5

// Blue special powers
#define BLUE_POWER_TELEPORT			6
#define BLUE_POWER_TURN				7
#define BLUE_POWER_LIGHTNING		8


// The height of the teleport power
#define TELEPORT_POWER_HEIGHT		15.0f


// Special power in progress? (zero == not using, 1 == p1 using, 2 == p2)
extern int using_special_power;

// Which special power
extern int which_special_power;

// Special power timer
extern int special_power_count;

// Does the special power pause the game?
extern bool special_power_pause;



void invoke_special_power(int who, int what);
void update_special_power();
void draw_special_power();




// A base class from which the special powers are derived
class SPECIAL_POWER {
public:
	virtual void init() = 0;				// Initialize the special power
	virtual void draw() = 0;				// Draw the special power
	virtual void update() = 0;				// Update the special power
};



// Trap power
class SP_TRAP : public SPECIAL_POWER {
public:
	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_TRAP sp_trap;


// Wildfire power
class SP_WILDFIRE : public SPECIAL_POWER {
public:
	ENEMY *burning[ENEMY_AMOUNT];			// Pointers to the burning enemies
	int burn_count[ENEMY_AMOUNT];			// Burning counters for the burning enemies

	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_WILDFIRE sp_wildfire;


// Napalm power
class SP_NAPALM : public SPECIAL_POWER {
public:
	int cur_x, cur_y;						// Current X and Y of the explosions
	int explo_delay;						// Delay between explosions
	int oldx[2], oldy[2];					// Old player positions before the jump

	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_NAPALM sp_napalm;


// Wisp power
class SP_WISP : public SPECIAL_POWER {
public:
	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_WISP sp_wisp;


// Potato man power
class SP_POTATOMAN : public SPECIAL_POWER {
public:
	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_POTATOMAN sp_potatoman;


// Flower power
class SP_FLOWERPOWER : public SPECIAL_POWER {
public:
	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_FLOWERPOWER sp_flowerpower;


// Teleport power
class SP_TELEPORT : public SPECIAL_POWER {
public:
	int who;								// Who is teleporting?
	int teleport_dir;						// Teleport direction
	float teleport_pos;						// Position in the teleport "beam" (between 0 and 1)
	int tx, ty;								// Target position

	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_TELEPORT sp_teleport;


// Turn power
class SP_TURN : public SPECIAL_POWER {
public:
	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_TURN sp_turn;


// Lightning power
class SP_LIGHTNING : public SPECIAL_POWER {
public:
	ENEMY *targets[ENEMY_AMOUNT];			// Target pointers
	float noise[ENEMY_AMOUNT];				// Noise levels for the lightnings
	int num_targets;						// Number of targets

	virtual void init();
	virtual void draw();
	virtual void update();
};
extern SP_LIGHTNING sp_lightning;




#endif
