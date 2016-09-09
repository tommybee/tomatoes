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

#ifndef INIT_H
#define INIT_H

#include "config.h"

// Keep a global pointer to the screen
struct SDL_Surface;
extern SDL_Surface *screen;

// Keep a global pointer to the config
extern CONFIG config;

// Keep a global pointer to the data file
struct MPAK_FILE;
extern MPAK_FILE pakfile;


// Initialize SDL and OpenGL
void init_sdl_and_gl();

// Display an error message and quit
void error_msg(const char *msg, ...);

#endif


