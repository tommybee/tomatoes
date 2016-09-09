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
#include "texture.h"
#include "init.h"
#include "font.h"
#include "select_special.h"
#include "special_power.h"
#include "player.h"
#include "game.h"
#include "mymath.h"
#include "comments.h"
#include "soundmusic.h"
#include "levels.h"
#include "helpers.h"


// Special selection icons
GLuint special_icons[3];

// Icon background texture
GLuint icon_bg;

// This structure holds the icon menu data
ICON_MENU icon_menu;

// This holds the time spent in the menu
static Uint32 menu_time;

// This holds the number of burning enemies when entered the menu
static int num_burning;

// Number of enemies when entered the menu
static int num_enemies_menu;

// From bonus.cpp
extern int killed_5_diamonds;


// Icon names
const char icon_names[NUM_ICONS][128] = { "Trap", "Wild Fire", "Napalm Strike", "Will O' The Wisp", "Potato Man", "Flower Power", "Teleport", "Turn", "Lightning Bolt" };

// Icon colors
const int icon_colors[NUM_ICONS] = { COL_RED, COL_RED, COL_RED, COL_GREEN, COL_GREEN, COL_GREEN, COL_BLUE, COL_BLUE, COL_BLUE };



// Draw the special selection icons
void draw_special_selection() {
	if(!icon_menu.visible && icon_menu.pos <= 0.05f)
		return;

	// Set smaller fonts
	set_font_scale(0.2f, 0.2f);

	// Translate to the correct position
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-NUM_ICONS * 0.5f, 3.45f - 1.1f * icon_menu.pos, -8.5f);	// 2.35f
	glTranslatef(0.5f, 0.5f, 0);

	// Texture coordinates
	float texx = 0.0f, texy = 0.0f;

	// Draw the icons
	for(int f=0; f<NUM_ICONS; f++) {
		// Determine if the icon is disabled
		bool disabled = (icon_menu.count[f] == 0 || (icon_menu.is_on_block && (f == RED_POWER_TRAP || f == GREEN_POWER_FLOWERPOWER)));
		if(f == GREEN_POWER_WISP && wisp.alive)
			disabled = true;
		if(f == GREEN_POWER_POTATOMAN && potatoman.alive)
			disabled = true;
		if(f == RED_POWER_WILDFIRE && (!num_enemies_menu || num_burning == 5))
			disabled = true;
		if(f == BLUE_POWER_TURN && !num_enemies_menu)
			disabled = true;
		if(f == BLUE_POWER_LIGHTNING && !num_enemies_menu)
			disabled = true;
		if(using_special_power && which_special_power == BLUE_POWER_LIGHTNING && f == BLUE_POWER_LIGHTNING)
			disabled = true;
		if(using_special_power && which_special_power == BLUE_POWER_LIGHTNING && f == BLUE_POWER_TURN)
			disabled = true;

		// Select the correct color
		float a = (f == icon_menu.selected) ? 0.8f : 0.6f;
		if(f == 0 || f == 1 || f == 2)
			glColor4f(1,.75f,.75f,a);
		else if(f == 3 || f == 4 || f == 5)
			glColor4f(.75f,1,.75f,a);
		else
			glColor4f(.75f,.75f,1,a);

		if(disabled)
			glColor4f(.5f,.5f,.5f,a);

		// Draw the background
		BIND_TEXTURE(icon_bg);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(1,1); glVertex2f( 0.5f,  0.5f);
			glTexCoord2f(0,1); glVertex2f(-0.5f,  0.5f);
			glTexCoord2f(1,0); glVertex2f( 0.5f, -0.5f);
			glTexCoord2f(0,0); glVertex2f(-0.5f, -0.5f);
		glEnd();

		// Set up the texture coordinates for the icon
		switch(f) {
			default:
			case 0:		// Trap
				texx = 0; texy = 0.5f; break;
			case 1:		// Wild fire
				texx = 0.5f; texy = 0.5f; break;
			case 2:		// Napalm
				texx = 0; texy = 0; break;
			case 3:		// Wisp
				texx = 0.5f; texy = 0; break;
			case 4:		// Potato man
				texx = 0; texy = 0.5f; break;
			case 5:		// Flower power
				texx = 0.5f; texy = 0.5f; break;
			case 6:		// Teleport
				texx = 0; texy = 0; break;
			case 7:		// Turn
				texx = 0.5f; texy = 0; break;
			case 8:		// Lightning
				texx = 0; texy = 0; break;
		}
		float t_add = 0.5f;
		if(f == 8)
			t_add = 1.0f;

		// Bind the correct texture
		if(f == 0 || f == 1 || f == 2 || f == 3)
			BIND_TEXTURE(special_icons[0]);
		else if(f == 4 || f == 5 || f == 6 || f == 7)
			BIND_TEXTURE(special_icons[1]);
		else
			BIND_TEXTURE(special_icons[2]);


		float col = 1.0f - 0.5f * (1.0f - icon_menu.anim[f]);
		//glColor4f(1,1,1,col);

		// Select the correct color
		float c = 0.6f + 0.4f * icon_menu.anim[f];
		if(f == 0 || f == 1 || f == 2)
			glColor4f(1,c,c,col);
		else if(f == 3 || f == 4 || f == 5)
			glColor4f(c,1,c,col);
		else
			glColor4f(c,c,1,col);

		if(disabled)
			glColor3f(.5f,.5f,.5f);


		// Draw the icon
		float size = 0.45f - 0.15f * (1.0f-icon_menu.anim[f]);
		if(disabled)
			size = 0.3f;
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(texx + t_add, texy + t_add); glVertex2f( size,  size);
			glTexCoord2f(texx, texy + t_add); glVertex2f(-size,  size);
			glTexCoord2f(texx + t_add, texy); glVertex2f( size, -size);
			glTexCoord2f(texx, texy); glVertex2f(-size, -size);
		glEnd();


		// Draw the amount of the corresponding bonuses
		glColor3f(1,1,0.7f);
		char str[5] = "";
		sprintf(str, "%2d", icon_menu.count[f]);
		glprint_num(-NUM_ICONS*0.5f + 0.55f + f, 3.55f - 1.1f*icon_menu.pos, -8.5f, str);


		// Translate right
		glTranslatef(1.0f, 0, 0);
	}

	glPopMatrix();

	// Restore the fonts
	set_font_scale(1,1);

}


// Handle the special selection menu
void do_special_selection() {
	int who = icon_menu.who - 1;

	if(icon_menu.wait > 0)
		icon_menu.wait--;

	// Scroll into the screen
	if(icon_menu.pos < 1.0f && icon_menu.visible)
		icon_menu.pos += 0.05f;
	else if(icon_menu.pos > 0 && !icon_menu.visible) {
		icon_menu.pos -= 0.05f;
		return;
	}

	if(!icon_menu.visible || level_pause)
		return;



	// Handle the animation
	int i = icon_menu.selected;
	if(icon_menu.anim[i] < 1.0f)
		icon_menu.anim[i] += 0.1f;

	for(int f=0; f<NUM_ICONS; f++)
		if(i != f && icon_menu.anim[f] > 0.0f)
			icon_menu.anim[f] -= 0.1f;

	// Handle the keyboard input
	if(key[config.key_left[who]]) {
		if(icon_menu.lkey_down == false) {
			icon_menu.lkey_down = true;
			icon_menu.selected--;
			if(icon_menu.selected < 0)
				icon_menu.selected = NUM_ICONS-1;

			add_comment(icon_colors[icon_menu.selected], (char*)icon_names[icon_menu.selected]);

			// Play the sound
			play_sound(SND_MENU1, false);
		}
	}
	else
		icon_menu.lkey_down = false;

	if(key[config.key_right[who]]) {
		if(icon_menu.rkey_down == false) {
			icon_menu.rkey_down = true;
			icon_menu.selected++;
			if(icon_menu.selected > NUM_ICONS-1)
				icon_menu.selected = 0;

			add_comment(icon_colors[icon_menu.selected], (char*)icon_names[icon_menu.selected]);

			// Play the sound
			play_sound(SND_MENU1, false);
		}
	}
	else
		icon_menu.rkey_down = false;


	// Invoke a special power
	if((key[config.key_shoot[who]] || key[SDLK_RETURN]) && icon_menu.wait == 0) {
		int power = icon_menu.selected;

		bool disabled = (icon_menu.count[power] == 0 || (icon_menu.is_on_block && (power == RED_POWER_TRAP || power == GREEN_POWER_FLOWERPOWER)));
		if(power == GREEN_POWER_WISP && wisp.alive)
			disabled = true;
		if(power == GREEN_POWER_POTATOMAN && potatoman.alive)
			disabled = true;
		if(power == RED_POWER_WILDFIRE && (!num_enemies_menu || num_burning == 5))
			disabled = true;
		if(power == BLUE_POWER_TURN && !num_enemies_menu)
			disabled = true;
		if(power == BLUE_POWER_LIGHTNING && !num_enemies_menu)
			disabled = true;
		if(using_special_power && which_special_power == BLUE_POWER_LIGHTNING && power == BLUE_POWER_LIGHTNING)
			disabled = true;
		if(using_special_power && which_special_power == BLUE_POWER_LIGHTNING && power == BLUE_POWER_TURN)
			disabled = true;

		if(disabled || (icon_menu.who == 1 && p1.dying))
			return;
		if(disabled || (icon_menu.who == 2 && p2.dying))
			return;

		// Use up the special power
		invoke_special_power(icon_menu.who, power);

		add_comment(COL_YELLOW, "%s chosen.", (char*)icon_names[power]);

		// Close the menu
		icon_menu.visible = false;
		icon_menu.wait = 15;

		// Restore the timing
		if(!killed_5_diamonds) {
			Uint32 diff = SDL_GetTicks() - menu_time;
			level_time += diff;
		}

		// Play the sound
		play_sound(SND_MENU2, false);
		return;
	}


	// Cancel the menu
	if(key[config.key_special[who]] && icon_menu.wait == 0) {
		icon_menu.visible = false;
		icon_menu.wait = 15;

		// Restore the timing
		if(!killed_5_diamonds) {
			Uint32 diff = SDL_GetTicks() - menu_time;
			level_time += diff;
		}

		// Play the sound
		play_sound(SND_MENU2, false);
	}

}



// Load the special selection icons
void load_icons() {
	special_icons[0] = load_png("icons1.png", true, false, false);
	special_icons[1] = load_png("icons2.png", true, false, false);
	special_icons[2] = load_png("icons3.png", true, false, false);
	icon_bg = load_png("iconbg.png", true, false, false);
}


// Open the icon menu
void open_icon_menu(int who, bool standing_on_block) {
	if(icon_menu.wait == 0) {
		icon_menu.visible = true;
		icon_menu.wait = 15;
		icon_menu.who = who;
		icon_menu.is_on_block = standing_on_block;

		// Choose the first enabled icon
		if(!icon_menu.count[icon_menu.selected]) {
			for(int f=0; f<NUM_ICONS; f++) {
				if(icon_menu.count[f]) {
					icon_menu.selected = f;
					break;
				}
			}
		}

		// Count the burning enemies
		num_burning = 0;
		list<ENEMY>::iterator e;
		for(e = enemylist.begin(); e != enemylist.end(); ++e)
			if((*e).burning)
				num_burning++;

		num_enemies_menu = enemylist.size();

		// Save the level timing
		menu_time = SDL_GetTicks();
	}
}


// Initialize the special selection icons
void init_special_selection() {
	// Select the first enabled icon
	icon_menu.selected = 0;
	int f;
	for(f=0; f<NUM_ICONS; f++) {
		if(icon_menu.count[f]) {
			icon_menu.selected = f;
			break;
		}
	}

	for(f=0; f<NUM_ICONS; f++)
		icon_menu.anim[f] = 0.0f;

	icon_menu.anim[icon_menu.selected] = 1.0f;

	icon_menu.visible = false;
	icon_menu.lkey_down = icon_menu.rkey_down = false;
	icon_menu.wait = 0;
	icon_menu.pos = 0;
	icon_menu.who = 0;
	icon_menu.is_on_block = false;
}

