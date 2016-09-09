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

#ifndef LEVELS_H
#define LEVELS_H

// Current level
extern int cur_level;

// Level time counter
extern Uint32 level_time;

// Number of seconds left
extern int num_seconds_left;

// Level change interval
extern const Uint32 level_change_interval;

// Is the level begin/end pause active?
#define LEVEL_PAUSE_BEGIN		1
#define LEVEL_PAUSE_END			2
extern int level_pause;


// Return the number of seconds left
float get_level_seconds_left();

// Change level.
// Return false if we're finished all the levels.
bool change_level();

// Init the levels
void init_levels();

// Find the levels
void find_levels();

// Switch to the next level
void next_level();

// Start timing the level change
void start_level_timing();


#endif

