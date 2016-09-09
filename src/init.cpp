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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "SDL.h"
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "SDL_opengl.h"


#include "config.h"
#include "mpak.h"
#include "mymath.h"


// Keep a global pointer to the screen
SDL_Surface *screen;


// Keep a global pointer to the config
CONFIG config;


// Keep a global pointer to the data file
MPAK_FILE pakfile;



// Display an error message and quit
void error_msg(const char *msg, ...) {

	char *buf = new char[4096];

	// Format the msg to buf
	va_list ap;
	va_start(ap, msg);
	vsprintf(buf, msg, ap);
	va_end(ap);

	// Output to stderr
	fprintf(stderr, (char*)"Error appeared:\n - %s\n", buf);

	SDL_Quit();

	// Display the message using MessageBoxes on Windows
#ifdef WIN32
	MessageBox(HWND_DESKTOP, buf, (char*)"Error appeared", MB_OK|MB_ICONERROR);
#endif

	delete [] buf;
	exit(-1);
}


// Initialize SDL and OpenGL
void init_sdl_and_gl() {

	// Load the config
	load_config(get_config_location(), &config);

	// Check for 8-bit
	if(config.vid_color_depth == 8)
		error_msg("Sorry, 8-bit color depth is not supported!\nYou must use 15, 16, 24 or 32-bit mode.\n");

	// Initialize SDL with video and timer support
	if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) < 0)
		error_msg("Unable to init SDL: %s", SDL_GetError());

	// Hide the mouse cursor
	SDL_ShowCursor(0);

	// Set window caption
	SDL_WM_SetCaption("I Have No Tomatoes", "I Have No Tomatoes");

	// Open the pakfile, with globally define OVERRIDE_DIR being the override directory
	pakfile.init();
	if(!pakfile.open_mpk(MPAK_READ, (char*)MPK_DIR "tomatoes.mpk", (char*)OVERRIDE_DIR))
		error_msg("Unable to open 'tomatoes.mpk'.\nThe file either doesn't exist or is corrupted.");

	// Load an icon
	SDL_Surface *icon = NULL;
	FILE *fin = pakfile.open_file((char*)"icon.bmp");
	if(!fin)
		error_msg("Unable to load the icon!\n");
	icon = SDL_LoadBMP_RW(SDL_RWFromFP(fin, 1), 1);
	if(!icon)
		error_msg("Unable to load the icon!\n");
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, SDL_MapRGB(icon->format, 255, 0, 255));
	SDL_WM_SetIcon(icon, NULL);
	SDL_FreeSurface(icon);

	// Initialize math tables
	init_math();

	// Set the video mode
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	screen = SDL_SetVideoMode(config.vid_w, config.vid_h, config.vid_color_depth, (config.fullscreen) ? (SDL_OPENGL|SDL_FULLSCREEN) : (SDL_OPENGL));
	if(screen == NULL)
		error_msg("Unable to set the OpenGL video mode %d x %d (%d bit)!\n%s", config.vid_w, config.vid_h, config.vid_color_depth, SDL_GetError());

	// Set OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

	glClearColor(0, 0, 0, 0);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	if(config.vid_color_depth == 32 || config.vid_color_depth == 24)
		glDisable(GL_DITHER);
	else
		glEnable(GL_DITHER);

	glViewport(0, 0, screen->w, screen->h);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	// Alpha test
	glAlphaFunc(GL_GREATER, 0.1f);
}


