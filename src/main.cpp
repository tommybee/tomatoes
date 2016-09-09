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
#include "init.h"
#include "mpak.h"
#include "game.h"
#include "timer.h"
#include "tilemap.h"
#include "player.h"
#include "font.h"
#include "texture.h"
#include "bomb.h"
#include "particle.h"
#include "enemy.h"
#include "bonus.h"
#include "teleport.h"
#include "soundmusic.h"
#include "trap.h"
#include "helpers.h"
#include "select_special.h"
#include "levels.h"
#include "menu.h"


// Logo texture
GLuint logo_tex;


// Show the loading screen
void loading_screen() {
	// Load the logo texture
	logo_tex = load_png((char*)"logotex.png", true, false, true);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glLoadIdentity();

	begin_fonts();

	// Draw the background
	BIND_TEXTURE(0);
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(0.0f,0.0f,0.0f);	// 0, 0.5f, 0.7f
		glVertex3f( 4, 3, -2);
		glVertex3f(-4, 3, -2);
		glColor3f(0, 0.4f, 0.7f);	// 0, 0.2f, 0.3f
		glVertex3f( 4,-3, -2);
		glVertex3f(-4,-3, -2);
	glEnd();


	// Draw the logo
	glEnable(GL_BLEND);
	BIND_TEXTURE(logo_tex);
	glTranslatef(0,1,-13);
	glColor3f(1,1,1);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 4, .5f, 0);
		glTexCoord2f(0,1); glVertex3f(-4, .5f, 0);
		glTexCoord2f(1,0); glVertex3f( 4,-.5f, 0);
		glTexCoord2f(0,0); glVertex3f(-4,-.5f, 0);
	glEnd();

	// Draw the text
	glColor3f(1,1,0.5f);
	glprintf_center(font1, 0, 0, -1, -20, (char*)"...LOADING...");
	end_fonts();

	glPopMatrix();

	// Flush and swap the buffers
	glFlush();
	SDL_GL_SwapBuffers();
}


// The good old main()
int main(int argc, char *argv[]) {
	// Initialize SDL and OpenGL
	init_sdl_and_gl();

	// Initialize FMOD
	init_fmod();

	// Load fonts
	build_font_list();
	font1 = load_png((char*)"fonts.png", true, false, true);

	// Show the loading screen
	loading_screen();

	// Find the level files
	find_levels();

	// Load textures
	font_num = load_png((char*)"fonts_num.png", true, false, true);
	load_bgrounds();
	load_players();
	load_enemies();
	load_bonus();
	load_maptex();
	load_bombs();
	load_particles();
	load_teleports();
	load_traps();
	load_wisps();
	load_potatoman();
	load_icons();
	load_menus();


	// Add timer to run 60 fps
	start_timer(60);

	// Menu logic
	int ans = show_menu(MENU_ID_MAIN);
	while(ans != MENU_EXIT) {
		// Start the game?
		if(ans == MENU_SINGLEPLAY)			// Single player
			start_game();
		else if(ans == MENU_MULTIPLAY)		// Two players
			start_game(true);

		// Show the menu again
		ans = show_menu(MENU_ID_MAIN);
	}


	// Destroy timer
	kill_timer();

	// Free stuff
	delete_font_list();
	kill_particles();


	// Kill SDL
	SDL_Quit();

	// Close the pakfile
	pakfile.close_mpk();

	// Close the FMOD
	FMUSIC_StopSong(music_mod);
	FMUSIC_FreeSong(music_mod);
	FSOUND_Close();

	// Save the config
	save_config(get_config_location(true), &config);
	return 0;
}


