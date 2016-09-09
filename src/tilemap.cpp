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
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "tilemap.h"
#include "texture.h"
#include "mymath.h"
#include "init.h"
#include "mpak.h"
#include "player.h"


// A dirty hack. Read the comments from the beginning of player.cpp
extern int players_on_block_x[2];
extern int players_on_block_y[2];



// Map display list
GLuint map_dlist;


// Map array with two layers
GLuint map[MAP_W][MAP_H][2];

// Teleport array
bool teleport_map[MAP_W][MAP_H];

// Teleport positions
int teleport_x[2] = { -1, -1 };
int teleport_y[2] = { -1, -1 };

// Teleport directions
int teleport_dir[2] = { 0, 0 };


// Map textures
GLuint maptex[NUM_TEX];

// Current tile roof texture
GLuint tile_roof_tex = 2;



// Load map textures
void load_maptex() {
	for(int f=0; f<NUM_TEX; f++) {
		char file[128] = "";
		sprintf(file, "maptex%02d.jpg", f+1);
		maptex[f] = load_jpg(file, false, true, true);
	}
}


// Can an enemy teleport at given point?
bool can_teleport(int x, int y, bool player) {
	// Check the boundaries
	if(x < 0 || y < 0 || x > MAP_W-1 || y > MAP_H-1)
		return false;

	// If the player is teleporting, check the other teleports
	if(player && ((x == teleport_x[0] && y == teleport_y[0]) || (x == teleport_x[1] && y == teleport_y[1])))
		return false;

	// Check the teleport areas
	if(teleport_map[x][y])
		return true;

	return false;
}


// Is the map solid at given point?
bool map_solid(int x, int y) {
	// Check the boundaries
	if(x < 0 || y < 0 || x > MAP_W-1 || y > MAP_H-1)
		return true;

	// Check the blocks first
	if(map[x][y][1])
		return true;

	// .. and then the floor tiles
	return bool(!map[x][y][0]);
}


// Check if there's a block
bool is_block_at(int x, int y) {
	if(x < 0 || y < 0 || x > MAP_W-1 || y > MAP_H-1)
		return false;

	return bool(map[x][y][1]);
}


// Build the map display list
void build_map() {

	kill_map();

	// Build the list
	map_dlist = glGenLists(1);
	glNewList(map_dlist, GL_COMPILE);

	glEnable(GL_LIGHTING);

	// Loop through the map
	for(int y=0; y<MAP_H; y++) {
		for(int x=0; x<MAP_W; x++) {

			// Draw the upper tile
			if(map[x][y][1]) {
				BIND_TEXTURE(maptex[tile_roof_tex]);

				// Top
				glColor3f(0.8f, 0.8f, 0.8f);
				glBegin(GL_TRIANGLE_STRIP);
					glNormal3f(0,1,0);
					glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y);
					glTexCoord2f(0,1); glVertex3f(x,TILE_H,y);
					glTexCoord2f(1,0); glVertex3f(x+1,TILE_H,y+1);
					glTexCoord2f(0,0); glVertex3f(x,TILE_H,y+1);
				glEnd();

				// Left
				BIND_TEXTURE(maptex[map[x][y][1] - 1]);
				glColor3f(1,1,1);
				if(!is_block_at(x,y+1)) {
					glBegin(GL_TRIANGLE_STRIP);
						glNormal3f(0,0,1);
						glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y+1);
						glTexCoord2f(0,1); glVertex3f(x,TILE_H,y+1);
						glTexCoord2f(1,0); glVertex3f(x+1,0.0f,y+1);
						glTexCoord2f(0,0); glVertex3f(x,0.0f,y+1);
					glEnd();
				}

				// Right
				if(!is_block_at(x+1,y)) {
					glBegin(GL_TRIANGLE_STRIP);
						glNormal3f(1,0,0);
						glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y);
						glTexCoord2f(0,1); glVertex3f(x+1,TILE_H,y+1);
						glTexCoord2f(1,0); glVertex3f(x+1,0.0f,y);
						glTexCoord2f(0,0); glVertex3f(x+1,0.0f,y+1);
					glEnd();
				}
			}

			// Draw the lower tile
			else if(map[x][y][0]) {
				glDepthMask(GL_FALSE);
				BIND_TEXTURE(maptex[map[x][y][0] - 1]);
				glBegin(GL_TRIANGLE_STRIP);
					glNormal3f(0,1,0);
					glTexCoord2f(1,1); glVertex3f(x+1,0,y);
					glTexCoord2f(0,1); glVertex3f(x,0,y);
					glTexCoord2f(1,0); glVertex3f(x+1,0,y+1);
					glTexCoord2f(0,0); glVertex3f(x,0,y+1);
				glEnd();
				glDepthMask(GL_TRUE);
			}

		}
	}

	glDisable(GL_LIGHTING);
	glEndList();

}


// Draw the map
void draw_map() {

	glColor3f(1,1,1);
#ifndef EDITOR
	// This is a dirty hack cos I can't use the display list when the player is on a block.
	if(players_on_block_x[0] == -1 && players_on_block_x[1] == -1)
		glCallList(map_dlist);
	else {
		// Draw the map dynamically and alter the depth writing of the tile where the player is.
		glEnable(GL_LIGHTING);

		// Loop through the map
		for(int y=0; y<MAP_H; y++) {
			for(int x=0; x<MAP_W; x++) {

				// Draw the upper tile
				if(map[x][y][1]) {
					// Is the player on this tile?
					bool player_over = ((players_on_block_x[0] == x && players_on_block_y[0] == y) || (players_on_block_x[1] == x && players_on_block_y[1] == y)) ? true : false;

					// Left
					BIND_TEXTURE(maptex[map[x][y][1] - 1]);
					glColor3f(1,1,1);
					if(!is_block_at(x,y+1)) {
						glBegin(GL_TRIANGLE_STRIP);
							glNormal3f(0,0,1);
							glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y+1);
							glTexCoord2f(0,1); glVertex3f(x,TILE_H,y+1);
							glTexCoord2f(1,0); glVertex3f(x+1,0.0f,y+1);
							glTexCoord2f(0,0); glVertex3f(x,0.0f,y+1);
						glEnd();
					}

					// Back left
					if(player_over) {
						glBegin(GL_TRIANGLE_STRIP);
							glNormal3f(0,0,1);
							glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y);
							glTexCoord2f(0,1); glVertex3f(x,TILE_H,y);
							glTexCoord2f(1,0); glVertex3f(x+1,0.0f,y);
							glTexCoord2f(0,0); glVertex3f(x,0.0f,y);
						glEnd();
					}

					// Right
					if(!is_block_at(x+1,y)) {
						glBegin(GL_TRIANGLE_STRIP);
							glNormal3f(1,0,0);
							glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y);
							glTexCoord2f(0,1); glVertex3f(x+1,TILE_H,y+1);
							glTexCoord2f(1,0); glVertex3f(x+1,0.0f,y);
							glTexCoord2f(0,0); glVertex3f(x+1,0.0f,y+1);
						glEnd();
					}

					// Back right
					if(player_over) {
						glBegin(GL_TRIANGLE_STRIP);
							glNormal3f(0,0,1);
							glTexCoord2f(1,1); glVertex3f(x,TILE_H,y);
							glTexCoord2f(0,1); glVertex3f(x,TILE_H,y+1);
							glTexCoord2f(1,0); glVertex3f(x,0.0f,y);
							glTexCoord2f(0,0); glVertex3f(x,0.0f,y+1);
						glEnd();
					}


					// Draw a lower plane that will do some cover up work
					// (e.g. the napalm explosions)
					if(player_over) {
						glBegin(GL_TRIANGLE_STRIP);
							glNormal3f(0,1,0);
							glTexCoord2f(1,1); glVertex3f(x+1,0.20f,y);
							glTexCoord2f(0,1); glVertex3f(x,0.20f,y);
							glTexCoord2f(1,0); glVertex3f(x+1,0.20f,y+1);
							glTexCoord2f(0,0); glVertex3f(x,0.20f,y+1);
						glEnd();
						glDepthMask(GL_FALSE);
					}

					// We draw the top last because it has it's depth writing disable occasionally
					glColor3f(0.8f, 0.8f, 0.8f);
					BIND_TEXTURE(maptex[tile_roof_tex]);
					glBegin(GL_TRIANGLE_STRIP);
						glNormal3f(0,1,0);
						glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y);
						glTexCoord2f(0,1); glVertex3f(x,TILE_H,y);
						glTexCoord2f(1,0); glVertex3f(x+1,TILE_H,y+1);
						glTexCoord2f(0,0); glVertex3f(x,TILE_H,y+1);
					glEnd();
					if(player_over) {
						glDepthMask(GL_TRUE);
					}
					glColor3f(1,1,1);
				}

				// Draw the lower tile
				else if(map[x][y][0]) {
					glDepthMask(GL_FALSE);
					BIND_TEXTURE(maptex[map[x][y][0] - 1]);
					glBegin(GL_TRIANGLE_STRIP);
						glNormal3f(0,1,0);
						glTexCoord2f(1,1); glVertex3f(x+1,0,y);
						glTexCoord2f(0,1); glVertex3f(x,0,y);
						glTexCoord2f(1,0); glVertex3f(x+1,0,y+1);
						glTexCoord2f(0,0); glVertex3f(x,0,y+1);
					glEnd();
					glDepthMask(GL_TRUE);
				}

			}
		}

		glDisable(GL_LIGHTING);
	}
#else
	glEnable(GL_LIGHTING);

	// Loop through the map
	for(int y=0; y<MAP_H; y++) {
		for(int x=0; x<MAP_W; x++) {

			// Draw the upper tile
			if(map[x][y][1]) {
				BIND_TEXTURE(maptex[tile_roof_tex]);

				// Top
				glColor3f(0.8f, 0.8f, 0.8f);
				glBegin(GL_TRIANGLE_STRIP);
					glNormal3f(0,1,0);
					glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y);
					glTexCoord2f(0,1); glVertex3f(x,TILE_H,y);
					glTexCoord2f(1,0); glVertex3f(x+1,TILE_H,y+1);
					glTexCoord2f(0,0); glVertex3f(x,TILE_H,y+1);
				glEnd();

				// Left
				BIND_TEXTURE(maptex[map[x][y][1] - 1]);
				glColor3f(1,1,1);
				if(!is_block_at(x,y+1)) {
					glBegin(GL_TRIANGLE_STRIP);
						glNormal3f(0,0,1);
						glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y+1);
						glTexCoord2f(0,1); glVertex3f(x,TILE_H,y+1);
						glTexCoord2f(1,0); glVertex3f(x+1,0.0f,y+1);
						glTexCoord2f(0,0); glVertex3f(x,0.0f,y+1);
					glEnd();
				}

				// Right
				if(!is_block_at(x+1,y)) {
					glBegin(GL_TRIANGLE_STRIP);
						glNormal3f(1,0,0);
						glTexCoord2f(1,1); glVertex3f(x+1,TILE_H,y);
						glTexCoord2f(0,1); glVertex3f(x+1,TILE_H,y+1);
						glTexCoord2f(1,0); glVertex3f(x+1,0.0f,y);
						glTexCoord2f(0,0); glVertex3f(x+1,0.0f,y+1);
					glEnd();
				}
			}

			// Draw the lower tile
			else if(map[x][y][0]) {
				glDepthMask(GL_FALSE);
				BIND_TEXTURE(maptex[map[x][y][0] - 1]);
				glBegin(GL_TRIANGLE_STRIP);
					glNormal3f(0,1,0);
					glTexCoord2f(1,1); glVertex3f(x+1,0,y);
					glTexCoord2f(0,1); glVertex3f(x,0,y);
					glTexCoord2f(1,0); glVertex3f(x+1,0,y+1);
					glTexCoord2f(0,0); glVertex3f(x,0,y+1);
				glEnd();
				glDepthMask(GL_TRUE);
			}

		    // Draw the teleport areas
		    if(teleport_map[x][y]) {
      			BIND_TEXTURE(0);
				glDepthMask(GL_FALSE);
				glDisable(GL_LIGHTING);
				glBegin(GL_TRIANGLES);
					glColor3f(.1f,.4f,.5f);
					glVertex3f(x+0.3f, 0, y+0.7f);
					glVertex3f(x+0.7f, 0, y+0.3f);
					glColor3f(.2f,.7f,1);
					glVertex3f(x+.5f, 0.8f, y+.5f);
				glEnd();
				/*glBegin(GL_TRIANGLE_STRIP);
					glVertex3f(x+0.75,0.3,y+0.25);
					glVertex3f(x+0.25,0.3,y+0.25);
					glVertex3f(x+0.75,0.3,y+0.75);
					glVertex3f(x+0.25,0.3,y+0.75);
				glEnd();
				*/
				glEnable(GL_LIGHTING);
				glDepthMask(GL_TRUE);
				glColor3f(1,1,1);

		    }

		}
	}

	glDisable(GL_LIGHTING);
#endif
}


// Destroy the map display list
void kill_map() {
	glDeleteLists(map_dlist, 1);
}


// Save the map to a file
void save_map(char *file) {
#ifdef EDITOR
	// Open the file
	FILE *f = fopen(file, "wb");
	if(!f)
		error_msg("Unable to save the level to %s!\n", file);

	// Save the ID
	fputc('M', f);
	fputc('H', f);
	fputc('!', f);

	// Save the player position
	fputc(p1.x, f);
	fputc(p1.y, f);

	// Save the player direction
	fputc(p1.dir, f);

	// Save the teleport positions and directions
	fputc(teleport_x[0] + 1, f);
	fputc(teleport_y[0] + 1, f);
	fputc(teleport_x[1] + 1, f);
	fputc(teleport_y[1] + 1, f);
	fputc(teleport_dir[0], f);
	fputc(teleport_dir[1], f);

	// Save the roof texture
	fputc(tile_roof_tex, f);


	// Save the map layers
	for(int y=0; y < MAP_H; y++) {
		for(int x=0; x < MAP_W; x++) {
			// Save the floors and blocks
			fputc(map[x][y][0], f);
			fputc(map[x][y][1], f);

			// Save the teleport marks
			fputc((teleport_map[x][y]) ? 1 : 0, f);
		}
	}

	// Close the file
	fclose(f);
#endif
}


// Load the map from a file
void load_map(char *file) {
#ifndef EDITOR
	// Load the level from the pakfile
	FILE *f = pakfile.open_file(file);
	if(!f)
		error_msg("Unable to load the level %s from the pakfile!\n", file);
#else
	// Open the file
	FILE *f = fopen(file, "rb");
	if(!f)
		return;
#endif

	// Check the ID
	char id[3];
	id[0] = fgetc(f);
	id[1] = fgetc(f);
	id[2] = fgetc(f);
	if(id[0] != 'M' || id[1] != 'H' || id[2] != '!')
		error_msg("'%s' is not a level file for this game!\n", file);

	// Get the player position
	p1.x = fgetc(f);
	p1.y = fgetc(f);
	p1.tx = p1.x;
	p1.ty = p1.y;
	p2.x = p2.tx = p1.x;
	p2.y = p2.ty = p1.y;

	// Get the player direction
	p1.dir = fgetc(f);
	p1.nextdir = p1.dir;
	p2.dir = p2.nextdir = p1.dir;

	// Get the teleport positions and directions
	teleport_x[0] = fgetc(f) - 1;
	teleport_y[0] = fgetc(f) - 1;
	teleport_x[1] = fgetc(f) - 1;
	teleport_y[1] = fgetc(f) - 1;
	teleport_dir[0] = fgetc(f);
	teleport_dir[1] = fgetc(f);

	// Get the roof texture
	tile_roof_tex = fgetc(f);


	// Load the map layers
	for(int y=0; y < MAP_H; y++) {
		for(int x=0; x < MAP_W; x++) {
			// Get the floors and blocks
			map[x][y][0] = fgetc(f);
			map[x][y][1] = fgetc(f);

			// Get the teleport marks
			teleport_map[x][y] = (fgetc(f)) ? true : false;
		}
	}

	// Close the file
	fclose(f);

	build_map();
}


// Clear the map
void clear_map() {
	for(int y=0; y<MAP_H; y++) {
		for(int x=0; x<MAP_W; x++) {
			map[x][y][0] = 1;
			map[x][y][1] = 0;
			teleport_map[x][y] = false;
		}
	}

	teleport_x[0] = teleport_y[1] = -1;
	teleport_x[1] = teleport_y[1] = -1;
	teleport_dir[0] = teleport_dir[1] = 0;
}


