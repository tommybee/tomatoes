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
#include <ctype.h>
#include <string.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "game.h"
#include "texture.h"
#include "init.h"
#include "timer.h"
#include "mymath.h"
#include "font.h"
#include "screenshot.h"
#include "soundmusic.h"
#include "bgrounds.h"
#include "mpak.h"
#include "menu.h"
#include "hiscore.h"


// Version
#define VERSION		"v1.5"


// Menu textures
GLuint button_tex;
GLuint button_tex2;
GLuint menu_bg;


// Item colors
float col_selected[3] = { 1, 1, 0.5f };
float col_selected2[3] = { 1, 1, 1 };
float col_normal[3] = { .75f, .75f, .75f };
float col_normal2[3] = { 0.0f, 0.8f, 1 };

// Fading stuff (from game.cpp)
// Are we fading? (0 == no, 1 == fade in, 2 == fade out)
extern int fading;
extern float fade_amount;


// Hiscore/credits stuff
float mid_fade_amount;
int mid_fade_dir;
int mid_state;		// 1 == single player hiscores, 2 == two player hiscores, 3 == credits
int mid_state_wait;


// Background animation
float back_anim;
float bx_roll, by_roll;
float bx_roll_dir, by_roll_dir;


// Key setting stuff
int whose_keys;
int *key_to_set;
int prev_key;
bool setting_key;




// Helper function which returns a key name in upper case
char *key_name(int key) {

	if(key != -1) {
		// Get the key name from SDL
		static char buf[32];
#ifdef WIN32
		_snprintf(buf, 32, "%s", SDL_GetKeyName((SDLKey)key));
#else
		snprintf(buf, 32, "%s", SDL_GetKeyName((SDLKey)key));
#endif

		if(strcmp(buf, "left") == 0)
			return (char*)"LEFT ARROW";
		else if(strcmp(buf, "right") == 0)
			return (char*)"RIGHT ARROW";
		else if(strcmp(buf, "up") == 0)
			return (char*)"UP ARROW";
		else if(strcmp(buf, "down") == 0)
			return (char*)"DOWN ARROW";

		// Convert it to upper case
		for(unsigned int c=0; c<strlen(buf); c++) {
			buf[c] = toupper(buf[c]);
		}

		return buf;
	}
	else
		return (char*)"PRESS A KEY";	// This is displayed when we're setting
								// a key.
}


// Helper function which draws the credits
void draw_credits(float fade) {
	set_font_scale(0.8f,0.7f);
	glColor4f(0.1f,1,1, fade);
	glprintf_center(font1, 0, 0, 3.05f, -13, (char*)"CREDITS");

	set_font_scale(0.6f, 0.6f);

	glColor4f(0.7f,0.8f,1, fade);
	glprintf_center(font1, 0, 0, 2.4f, -13, (char*)"GAME DESIGN");
	glColor4f(1,1,1, fade);
	glprintf_center(font1, 0, 0, 1.8f, -13, (char*)"TEEMU RUOKOLAINEN");

	glColor4f(0.7f,0.8f,1, fade);
	glprintf_center(font1, 0, 0, 0.8f, -13, (char*)"PROGRAMMING");
	glColor4f(1,1,1, fade);
	glprintf_center(font1, 0, 0, 0.2f, -13, (char*)"MIKA HALTTUNEN");

	glColor4f(0.7f,0.8f,1, fade);
	glprintf_center(font1, 0, 0, -0.8f, -13, (char*)"SOUND AND MUSIC");
	glColor4f(1,1,1, fade);
	glprintf_center(font1, 0, 0, -1.4f, -13, (char*)"TEEMU RUOKOLAINEN");

	glColor4f(0.7f,0.8f,1, fade);
	glprintf_center(font1, 0, 0, -2.4f, -13, (char*)"ARTWORK");
	glColor4f(1,1,1, fade);
	glprintf_center(font1, 0, 0, -3.0f, -13, (char*)"MIKA HALTTUNEN");

	set_font_scale(1,1);
}


// Draw the menu
void draw_menu(int menu_id, int menu_item, int place, float fade, HISCORE_LIST *list) {
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);

	// Set up the viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0f, 1.333333f, 1, 100);
	glTranslatef(0,0,-32);

	// Draw the background
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	BIND_TEXTURE(menu_bg);
	glDepthMask(GL_FALSE);
	float bx = bx_roll;
	float by = by_roll;
	glBegin(GL_TRIANGLE_STRIP);
		glColor3f(.1f,1,1);
		glTexCoord2f(-bx + 1,-by + 1); glVertex3f(14, 14, -2);
		glTexCoord2f(-bx + 0,-by + 1); glVertex3f(-14, 14, -2);
		glColor3f(0,0.4f,0.7f);
		glTexCoord2f(-bx + 1,-by + 0); glVertex3f(14, -14, -2);
		glTexCoord2f(-bx + 0,-by + 0); glVertex3f(-14, -14, -2);
	glEnd();
	float rot = -5.0f;
	glEnable(GL_BLEND);
	for(int f=0; f<5; f++) {
		glRotatef(rot * SIN(back_anim), 0,0,1);
		glBegin(GL_TRIANGLE_STRIP);
			glColor4f(0.1f,1,1,.25f);
			glTexCoord2f(-bx + 1,-by + 1); glVertex3f(14, 14, -2);
			glTexCoord2f(-bx + 0,-by + 1); glVertex3f(-14, 14, -2);
			glColor4f(0,0.4f,0.7f,.25f);
			glTexCoord2f(-bx + 1,-by + 0); glVertex3f(14, -14, -2);
			glTexCoord2f(-bx + 0,-by + 0); glVertex3f(-14, -14, -2);
		glEnd();
		rot += (1.0f * COS(back_anim));
	}

	glDepthMask(GL_TRUE);


	// Begin font drawing
	begin_fonts();
	glLoadIdentity();

	// Draw the logo
	glEnable(GL_BLEND);
	BIND_TEXTURE(logo_tex);
	glPushMatrix();
	glTranslatef(0,4.5f,-13);
	glRotatef(20.0f * COS(back_anim), 1,0,0);
	glColor3f(1,1,1);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 4, .5f, 0);
		glTexCoord2f(0,1); glVertex3f(-4, .5f, 0);
		glTexCoord2f(1,0); glVertex3f( 4,-.5f, 0);
		glTexCoord2f(0,0); glVertex3f(-4,-.5f, 0);
	glEnd();
	glPopMatrix();


	// Draw the hiscores/credits
	if(menu_id == MENU_ID_MAIN || menu_id == MENU_ID_START) {
		if(mid_state == 1)
			hiscore_1.draw(-1, mid_fade_amount);
		else if(mid_state == 2)
			hiscore_2.draw(-1, mid_fade_amount);
		else if(mid_state == 3)
			draw_credits(mid_fade_amount);
	}

	// Draw the version number
	set_font_scale(0.3f, 0.3f);
	glColor4f(1,1,1,0.3f);
	glprintf(font1, 0, -7.0f, -5.3f, -13, (char*)VERSION);
	set_font_scale(1,1);


	// Draw the hilighted hiscore list if we're typing a name
	if(place != -1 && list != NULL)
		list->draw(place, 1.0f);


	// Draw the menuitem background stripe
	BIND_TEXTURE(0);
	glColor4f(0,0,0,0.1f);
	glPushMatrix();
	glTranslatef(0,-4.0f, -13);
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(10,.57f,0);
		glVertex3f(-10,.57f,0);
		glVertex3f(10,-.57f,0);
		glVertex3f(-10,-.57f,0);
	glEnd();
	glPopMatrix();

	// Draw the menuitems
	if(menu_id == MENU_ID_MAIN) {
		// START
		if(menu_item == MENU_START)
			glColor3fv(col_selected);
		else
			glColor3fv(col_normal);
		BIND_TEXTURE(button_tex);
		glPushMatrix();
		glTranslatef(-4.0f, -4.0f, -13);
		glRotatef(20.0f * SIN(back_anim), 1,0,0);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1,1); glVertex3f( 2, .5f, 0);
			glTexCoord2f(0,1); glVertex3f(-2, .5f, 0);
			glTexCoord2f(1,.75f); glVertex3f( 2,-.5f, 0);
			glTexCoord2f(0,.75f); glVertex3f(-2,-.5f, 0);
		glEnd();
		glPopMatrix();

		// OPTIONS
		if(menu_item == MENU_OPTIONS)
			glColor3fv(col_selected);
		else
			glColor3fv(col_normal);
		glPushMatrix();
		glTranslatef(0, -4.0f, -13);
		glRotatef(20.0f * SIN(back_anim), 1,0,0);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1,.75f); glVertex3f( 2, .5f, 0);
			glTexCoord2f(0,.75f); glVertex3f(-2, .5f, 0);
			glTexCoord2f(1,.5f); glVertex3f( 2,-.5f, 0);
			glTexCoord2f(0,.5f); glVertex3f(-2,-.5f, 0);
		glEnd();
		glPopMatrix();


		// EXIT
		if(menu_item == MENU_EXIT)
			glColor3fv(col_selected);
		else
			glColor3fv(col_normal);
		glPushMatrix();
		glTranslatef(4.0f, -4.0f, -13);
		glRotatef(20.0f * SIN(back_anim), 1,0,0);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1,.5f); glVertex3f( 2, .5f, 0);
			glTexCoord2f(0,.5f); glVertex3f(-2, .5f, 0);
			glTexCoord2f(1,.25f); glVertex3f( 2,-.5f, 0);
			glTexCoord2f(0,.25f); glVertex3f(-2,-.5f, 0);
		glEnd();
		glPopMatrix();
	}
	else if(menu_id == MENU_ID_START) {
		// SINGLE PLAYER
		if(menu_item == MENU_SINGLEPLAY)
			glColor3fv(col_selected);
		else
			glColor3fv(col_normal);
		BIND_TEXTURE(button_tex2);
		glPushMatrix();
		glTranslatef(-2.0f, -4.0f, -13);
		glRotatef(20.0f * SIN(back_anim), 1,0,0);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1,1); glVertex3f( 2, .85f, 0);
			glTexCoord2f(0,1); glVertex3f(-2, .85f, 0);
			glTexCoord2f(1,.5f); glVertex3f( 2,-.85f, 0);
			glTexCoord2f(0,.5f); glVertex3f(-2,-.85f, 0);
		glEnd();
		glPopMatrix();

		// TWO PLAYERS
		if(menu_item == MENU_MULTIPLAY)
			glColor3fv(col_selected);
		else
			glColor3fv(col_normal);
		glPushMatrix();
		glTranslatef(2.0f, -4.0f, -13);
		glRotatef(20.0f * SIN(back_anim), 1,0,0);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1,.5f); glVertex3f( 2, .85f, 0);
			glTexCoord2f(0,.5f); glVertex3f(-2, .85f, 0);
			glTexCoord2f(1,0); glVertex3f( 2,-.85f, 0);
			glTexCoord2f(0,0); glVertex3f(-2,-.85f, 0);
		glEnd();
		glPopMatrix();
	}
	else if(menu_id == MENU_ID_OPTIONS) {
		int who = whose_keys;

		// Settings for ...
		set_font_scale(0.6f, 0.5f);
		if(menu_item == MENU_WHOSEKEYS)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		glprintf(font1, 0, -5.0f, 3.0f, -13, (char*)"Settings for:        %s", (whose_keys == 0) ? (char*)"PLAYER ONE" : (char*)"PLAYER TWO");

		// Moving style
		if(menu_item == MENU_MOVSTYLE)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		glprintf(font1, 0, -5.0f, 2.5f, -13, (char*)"Moving style:        %s", (config.moving_style[who] == 1) ? (char*)"RELATIVE" : (char*)"ABSOLUTE");

		// Key up
		if(menu_item == MENU_KEYUP)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		if(config.moving_style[who] == 1)
			glprintf(font1, 0, -5.0f, 2.0f, -13, (char*)"Move forward:        %s", key_name(config.key_up[who]));
		else
			glprintf(font1, 0, -5.0f, 2.0f, -13, (char*)"Move up:             %s", key_name(config.key_up[who]));

		// Key down
		if(menu_item == MENU_KEYDOWN)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		if(config.moving_style[who] == 1)
			glprintf(font1, 0, -5.0f, 1.5f, -13, (char*)"Turn around:         %s", key_name(config.key_down[who]));
		else
			glprintf(font1, 0, -5.0f, 1.5f, -13, (char*)"Move down:           %s", key_name(config.key_down[who]));

		// Key left
		if(menu_item == MENU_KEYLEFT)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		if(config.moving_style[who] == 1)
			glprintf(font1, 0, -5.0f, 1.0f, -13, (char*)"Turn left:           %s", key_name(config.key_left[who]));
		else
			glprintf(font1, 0, -5.0f, 1.0f, -13, (char*)"Move left:           %s", key_name(config.key_left[who]));

		// Key right
		if(menu_item == MENU_KEYRIGHT)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		if(config.moving_style[who] == 1)
			glprintf(font1, 0, -5.0f, 0.5f, -13, (char*)"Turn right:          %s", key_name(config.key_right[who]));
		else
			glprintf(font1, 0, -5.0f, 0.5f, -13, (char*)"Move right:          %s", key_name(config.key_right[who]));

		// Key bomb
		if(menu_item == MENU_KEYBOMB)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		glprintf(font1, 0, -5.0f, 0.0f, -13, (char*)"Throw a bomb:        %s", key_name(config.key_shoot[who]));

		// Key special
		if(menu_item == MENU_KEYSPECIAL)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		glprintf(font1, 0, -5.0f, -0.5f, -13, (char*)"Use a special:       %s", key_name(config.key_special[who]));

		// Sound volume slider
		if(menu_item == MENU_SOUNDVOL)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		glprintf(font1, 0, -5.0f, -1.5f, -13, (char*)"Sound volume:");
		glPushMatrix();
		glTranslatef(2.0f, -1.5f, -13);
		glColor3f(0,0,0.5f);
		BIND_TEXTURE(0);
		glBegin(GL_TRIANGLE_STRIP);
			glColor3f(0,0,0);
			glVertex2f(3, 0.55f);
			glVertex2f(0, 0.55f);
			glColor3f(0,0,0.5f);
			glVertex2f(3, 0.0f);
			glVertex2f(0, 0.0f);
		glEnd();
		glColor3f(0.5f,0.5f,1);
		float vol = (float)config.sound_vol / 255.0f;
		glBegin(GL_TRIANGLE_STRIP);
			glColor3f(0.5f,0.5f,1);
			glVertex2f(2.95f * vol, 0.50f);
			glVertex2f(0.05f, 0.50f);
			glColor3f(0.25f,0.25f,0.5f);
			glVertex2f(2.95f * vol, 0.05f);
			glVertex2f(0.05f, 0.05f);
		glEnd();
		glPopMatrix();

		// Music volume slider
		if(menu_item == MENU_MUSICVOL)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		glprintf(font1, 0, -5.0f, -2.0f, -13, (char*)"Music volume:");
		glPushMatrix();
		glTranslatef(2.0f, -2.0f, -13);
		BIND_TEXTURE(0);
		glBegin(GL_TRIANGLE_STRIP);
			glColor3f(0,0,0);
			glVertex2f(3, 0.55f);
			glVertex2f(0, 0.55f);
			glColor3f(0,0,0.5f);
			glVertex2f(3, 0.0f);
			glVertex2f(0, 0.0f);
		glEnd();
		vol = (float)config.music_vol / 255.0f;
		glBegin(GL_TRIANGLE_STRIP);
			glColor3f(0.5f,0.5f,1);
			glVertex2f(2.95f * vol, 0.50f);
			glVertex2f(0.05f, 0.50f);
			glColor3f(0.25f,0.25f,0.5f);
			glVertex2f(2.95f * vol, 0.05f);
			glVertex2f(0.05f, 0.05f);
		glEnd();
		glPopMatrix();


		// Toggle perspective mode
		if(menu_item == MENU_PERSPECTIVE)
			glColor3fv(col_selected2);
		else
			glColor3fv(col_normal2);
		glprintf(font1, 0, -5.0f, -3.0f, -13, (char*)"Perspective mode:    %s", (config.perspective_mode) ? (char*)"ON" : (char*)"OFF");


		set_font_scale(1,1);

		// OK
		if(menu_item == MENU_OK)
			glColor3fv(col_selected);
		else
			glColor3fv(col_normal);
		BIND_TEXTURE(button_tex);
		glPushMatrix();
		glTranslatef(-2.25f, -4.0f, -13);
		glRotatef(20.0f * SIN(back_anim), 1,0,0);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(.25f,.25f); glVertex3f( .5f, .5f, 0);
			glTexCoord2f(0,.25f); glVertex3f(-.5f, .5f, 0);
			glTexCoord2f(.25f,0); glVertex3f( .5f,-.5f, 0);
			glTexCoord2f(0,0); glVertex3f(-.5f,-.5f, 0);
		glEnd();
		glPopMatrix();

		// CANCEL
		if(menu_item == MENU_CANCEL)
			glColor3fv(col_selected);
		else
			glColor3fv(col_normal);
		glPushMatrix();
		glTranslatef(2.25f, -4.0f, -13);
		glRotatef(20.0f * SIN(back_anim), 1,0,0);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1,.25f); glVertex3f( 1.5f, .5f, 0);
			glTexCoord2f(.25f,.25f); glVertex3f(-1.5f, .5f, 0);
			glTexCoord2f(1,0); glVertex3f( 1.5f,-.5f, 0);
			glTexCoord2f(.25f,0); glVertex3f(-1.5f,-.5f, 0);
		glEnd();
		glPopMatrix();
	}

	// Draw the fade
	if(fade != 0.0f) {
		glLoadIdentity();
		BIND_TEXTURE(0);
		glColor4f(0,0,0,fade_amount);
		glBegin(GL_TRIANGLE_STRIP);
			glVertex3f(4, 3, -2);
			glVertex3f(-4, 3, -2);
			glVertex3f(4,-3,-2);
			glVertex3f(-4,-3,-2);
		glEnd();
	}

	end_fonts();
}


// Show a menu. Returns the action taken.
int show_menu(int menu_id) {
	// Initialize
	int action = 0;
	int menu_item = MENU_START;
	back_anim = 0.0f;
	bx_roll = RANDF(0,1);
	by_roll = RANDF(0,1);
	bx_roll_dir = RANDF(-0.001f,0.001f);
	by_roll_dir = RANDF(-0.001f,0.001f);
	mid_fade_amount = 0.0f;
	mid_fade_dir = 1;
	mid_state = 1;
	mid_state_wait = 3 * 60;
	memset(key, 0, sizeof(key));
	setting_key = false;
	key_to_set = NULL;
	whose_keys = 0;
	prev_key = 0;

	// Load the hiscores
	hiscore_1.load(get_hiscore_location(1));
	hiscore_2.load(get_hiscore_location(2));

	// Fade in
	fading = 1;
	fade_amount = 1.0f;

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);


	// Menu loop
	bool menu_loop = true;
	timer_count = 0;
	while(menu_loop) {
		// Handle events
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:		// Quit
					menu_loop = false;
					action = MENU_EXIT;
					break;

				case SDL_KEYDOWN:
					// Update the 'key' array
					if(setting_key == false)
						key[event.key.keysym.sym] = 1;
					// Set the key if it's not ESC, F12, F1
					else if(setting_key == true && event.key.keysym.sym != SDLK_ESCAPE && event.key.keysym.sym != SDLK_F12 && event.key.keysym.sym != SDLK_F1) {
						(*key_to_set) = event.key.keysym.sym;
						setting_key = false;
					}
					// ESC pressed -> cancel the key setting
					else if(setting_key == true && event.key.keysym.sym == SDLK_ESCAPE) {
						(*key_to_set) = prev_key;
						setting_key = false;
					}
					break;

				case SDL_KEYUP:
					// Update the 'key' array
					key[event.key.keysym.sym] = 0;
					break;
			}
		}

		while(timer_count > 0) {
			// If the current music has finished, start playing another
			check_music_finished();

			// Fade
			if(fading) {
				if(fading == 1) {
					// Fade in
					fade_amount -= 0.015f;
					if(fade_amount <= 0.0f) {
						fading = 0;
						fade_amount = 0.0f;
					}
				}
				else if(fading == 2) {
					// Fade out
					fade_amount += 0.015f;
					if(fade_amount >= 1.0f) {
						fading = 0;
						fade_amount = 1.0f;
						menu_loop = false;
					}
				}
			}

			// Take a screenshot?
			if(key[SDLK_F12])
				save_screenshot();

			// Handle the cursor movement
			if(key[SDLK_UP] || (key[SDLK_LEFT] && menu_item != MENU_MUSICVOL && menu_item != MENU_SOUNDVOL)) {
				play_sound(SND_MENU1, false);
				menu_item--;
				if(menu_id == MENU_ID_MAIN) {
					if(menu_item < MENU_START)
						menu_item = MENU_EXIT;
				}
				else if(menu_id == MENU_ID_START) {
					if(menu_item < MENU_SINGLEPLAY)
						menu_item = MENU_MULTIPLAY;
				}
				else if(menu_id == MENU_ID_OPTIONS) {
					if(menu_item < MENU_WHOSEKEYS)
						menu_item = MENU_CANCEL;
				}
			}

			if(key[SDLK_DOWN] || (key[SDLK_RIGHT] && menu_item != MENU_MUSICVOL && menu_item != MENU_SOUNDVOL)) {
				play_sound(SND_MENU1, false);
				menu_item++;
				if(menu_id == MENU_ID_MAIN) {
					if(menu_item > MENU_EXIT)
						menu_item = MENU_START;
				}
				else if(menu_id == MENU_ID_START) {
					if(menu_item > MENU_MULTIPLAY)
						menu_item = MENU_SINGLEPLAY;
				}
				else if(menu_id == MENU_ID_OPTIONS) {
					if(menu_item > MENU_CANCEL)
						menu_item = MENU_WHOSEKEYS;
				}
			}

			// Handle the ESC pressings
			if(key[SDLK_ESCAPE]) {
				play_sound(SND_MENU2, false);
				// In the main menu -> exit, otherwise return to the main menu
				if(menu_id == MENU_ID_MAIN) {
					action = MENU_EXIT;
					fading = 2;		// Fade out
				}
				if(menu_id == MENU_ID_START) {
					menu_id = MENU_ID_MAIN;
					menu_item = MENU_START;
				}
				if(menu_id == MENU_ID_OPTIONS) {
					menu_id = MENU_ID_MAIN;
					menu_item = MENU_OPTIONS;

					mid_fade_amount = 0.0f;
					mid_fade_dir = 1;
					mid_state = 1;
					mid_state_wait = 3 * 60;

					// Restore the settings
					load_config(get_config_location(), &config);
					FSOUND_SetSFXMasterVolume(config.sound_vol);
					FMUSIC_SetMasterVolume(music_mod, config.music_vol);
				}
			}

			// Handle the item selection
			if(key[SDLK_RETURN] || key[SDLK_SPACE]) {
				play_sound(SND_MENU2, false);
				switch(menu_item) {
					// MAIN MENU items
					case MENU_START:			// Start menu
						menu_id = MENU_ID_START;
						menu_item = MENU_SINGLEPLAY;
						break;
					case MENU_OPTIONS:			// Options menu
						menu_id = MENU_ID_OPTIONS;
						menu_item = MENU_WHOSEKEYS;
						break;
					case MENU_EXIT:				// Exit the game
						action = MENU_EXIT;
						fading = 2;				// Fade out
						break;

					// START MENU items
					case MENU_SINGLEPLAY:		// Start a single player game
						action = MENU_SINGLEPLAY;
						fading = 2;				// Fade out;
						break;
					case MENU_MULTIPLAY:		// Start a two player game
						action = MENU_MULTIPLAY;
						fading = 2;				// Fade out
						break;

					// OPTIONS MENU items
					case MENU_WHOSEKEYS:		// Toggle the player whose keys we're setting
						if(whose_keys == 0)
							whose_keys = 1;
						else if(whose_keys == 1)
							whose_keys = 0;
						break;
					case MENU_MOVSTYLE:			// Toggle the moving style
						if(config.moving_style[whose_keys] == 1)
							config.moving_style[whose_keys] = 2;
						else if(config.moving_style[whose_keys] == 2)
							config.moving_style[whose_keys] = 1;
						break;
					case MENU_KEYUP:			// Set the key up
						key_to_set = &(config.key_up[whose_keys]);
						setting_key = true;
						prev_key = config.key_up[whose_keys];
						config.key_up[whose_keys] = -1;
						break;

					case MENU_KEYDOWN:			// Set the key down
						key_to_set = &(config.key_down[whose_keys]);
						setting_key = true;
						prev_key = config.key_down[whose_keys];
						config.key_down[whose_keys] = -1;
						break;

					case MENU_KEYLEFT:			// Set the key left
						key_to_set = &(config.key_left[whose_keys]);
						setting_key = true;
						prev_key = config.key_left[whose_keys];
						config.key_left[whose_keys] = -1;
						break;

					case MENU_KEYRIGHT:			// Set the key right
						key_to_set = &(config.key_right[whose_keys]);
						setting_key = true;
						prev_key = config.key_right[whose_keys];
						config.key_right[whose_keys] = -1;
						break;

					case MENU_KEYBOMB:			// Set the key bomb
						key_to_set = &(config.key_shoot[whose_keys]);
						setting_key = true;
						prev_key = config.key_shoot[whose_keys];
						config.key_shoot[whose_keys] = -1;
						break;

					case MENU_KEYSPECIAL:		// Set the key special
						key_to_set = &(config.key_special[whose_keys]);
						setting_key = true;
						prev_key = config.key_special[whose_keys];
						config.key_special[whose_keys] = -1;
						break;

					case MENU_PERSPECTIVE:		// Toggle the perspective mode
						config.perspective_mode = !config.perspective_mode;
						break;

					case MENU_OK:				// Save the changes
						menu_id = MENU_ID_MAIN;
						menu_item = MENU_OPTIONS;

						mid_fade_amount = 0.0f;
						mid_fade_dir = 1;
						mid_state = 1;
						mid_state_wait = 3 * 60;

						save_config(get_config_location(true), &config);
						break;

					case MENU_CANCEL:			// Cancel the changes
						menu_id = MENU_ID_MAIN;
						menu_item = MENU_OPTIONS;

						mid_fade_amount = 0.0f;
						mid_fade_dir = 1;
						mid_state = 1;
						mid_state_wait = 3 * 60;

						// Restore the settings
						load_config(get_config_location(), &config);
						FSOUND_SetSFXMasterVolume(config.sound_vol);
						FMUSIC_SetMasterVolume(music_mod, config.music_vol);
				}
			}

			// Check the volume level sliders
			if(menu_item == MENU_MUSICVOL) {
				if(key[SDLK_LEFT]) {
					config.music_vol -= 10;
					if(config.music_vol < 0)
						config.music_vol = 0;
					// Update the volume levels
					FMUSIC_SetMasterVolume(music_mod, config.music_vol);
				}
				if(key[SDLK_RIGHT]) {
					config.music_vol += 10;
					if(config.music_vol > 255)
						config.music_vol = 255;
					// Update the volume levels
					FMUSIC_SetMasterVolume(music_mod, config.music_vol);
				}
			}
			if(menu_item == MENU_SOUNDVOL) {
				if(key[SDLK_LEFT]) {
					config.sound_vol -= 10;
					if(config.sound_vol < 0)
						config.sound_vol = 0;
					// Update the volume levels
					FSOUND_SetSFXMasterVolume(config.sound_vol);
					play_sound(SND_MENU1, false);
				}
				if(key[SDLK_RIGHT]) {
					config.sound_vol += 10;
					if(config.sound_vol > 255)
						config.sound_vol = 255;
					// Update the volume levels
					FSOUND_SetSFXMasterVolume(config.sound_vol);
					play_sound(SND_MENU1, false);
				}
			}


			// Clear the key array
			memset(key, 0, sizeof(key));

			// Animate the background
			back_anim = add_angle(back_anim, 2.0f);
			bx_roll += bx_roll_dir;
			if(bx_roll > 1.0f)
				bx_roll -= 1.0f;
			else if(bx_roll < 0.0f)
				bx_roll += 1.0f;

			by_roll += by_roll_dir;
			if(by_roll > 1.0f)
				by_roll -= 1.0f;
			else if(by_roll < 0.0f)
				by_roll += 1.0f;

			// Handle the hiscores/credits
			if(menu_id == MENU_ID_MAIN || menu_id == MENU_ID_START) {
				if(mid_state_wait > 0)
					mid_state_wait--;
				else if(mid_state_wait == 0) {
					// Fade either in or out
					if(mid_fade_dir == 1) {
						if(mid_fade_amount < 1.0f)
							mid_fade_amount += 0.008f;
						if(mid_fade_amount >= 1.0f) {
							// Wait a bit before fading out
							mid_fade_dir = 2;
							mid_state_wait = 5 * 60;
						}
					}
					else if(mid_fade_dir == 2) {
						if(mid_fade_amount > 0.0f)
							mid_fade_amount -= 0.008f;
						if(mid_fade_amount <= 0.0f) {
							// Wait a bit before changing the state and fading in
							mid_fade_dir = 1;
							mid_state_wait = 3 * 60;
							mid_state++;
							if(mid_state > 3)
								mid_state = 1;
						}
					}
				}
			}

			timer_count--;
		}


		// Draw the menu
		draw_menu(menu_id, menu_item, -1, fade_amount, NULL);

		// Flush and swap the buffers
		glFlush();
		SDL_GL_SwapBuffers();

	}

	SDL_EnableKeyRepeat(0,0);
	return action;
}



// Load the menu textures
void load_menus() {
	button_tex = load_png("menubut.png", true, false, true);
	button_tex2 = load_png("menubut2.png", true, false, true);
	menu_bg = load_jpg("menubg.jpg", false, true, true);
}

