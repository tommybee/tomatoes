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

#ifndef EFFECTS_H
#define EFFECTS_H

#include "vect.h"

// Explosion types
#define EXP_BOMB_NORMAL			0
#define EXP_BOMB_CENTER			1
#define EXP_NAPALM				2
#define EXP_BOMB_FLOWER			3
#define EXP_BOMB_CENTER_FLOWER	4


void create_teleport_effect(int x, int y);
void create_fire(VECT pos);
void create_explosion(int x, int y, int type);
void draw_lightning(VECT pos1, VECT pos2, float noise1, float noise2);

#endif

