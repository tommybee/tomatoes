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

#ifndef TILEMAP_H
#define TILEMAP_H

// Map dimensions
#define MAP_W			15
#define MAP_H			15

// Number of textures
#define NUM_TEX			54


// Tile height
#define TILE_H		0.6f


// Map textures
extern GLuint maptex[NUM_TEX];

// Map array with two layers
extern GLuint map[MAP_W][MAP_H][2];

// Teleport array
extern bool teleport_map[MAP_W][MAP_H];

// Teleport positions
extern int teleport_x[2];
extern int teleport_y[2];

// Teleport directions
extern int teleport_dir[2];



// Current tile roof texture
extern GLuint tile_roof_tex;


// Is the map solid at given point?
bool map_solid(int x, int y);

// Can an enemy teleport at given point?
bool can_teleport(int x, int y, bool player = false);

void load_maptex();
void draw_map();
void clear_map();
void build_map();
void kill_map();

void save_map(char *file);
void load_map(char *file);


#endif

