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

#include <stdlib.h>
#include "SDL.h"
#include "timer.h"
#include "init.h"

// Timer
int timer_count;
SDL_TimerID timer;

// Is the game paused?
bool game_paused;


// Timer callback
Uint32 timer_callback(Uint32 interval, void *param) {
	if(!game_paused)
		timer_count++;
	return interval;
}


// Start a timer
void start_timer(int fps) {

	// Add a timer to run at 'fps' frames per second
	timer_count = 0;
	game_paused = false;
	timer = SDL_AddTimer((1000/fps), timer_callback, 0);
	if(timer == NULL)
		error_msg("Error setting timer!\n%s", SDL_GetError());
}


// Destroy a timer
void kill_timer() {

	SDL_RemoveTimer(timer);
}


