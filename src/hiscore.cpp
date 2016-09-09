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
#include <string.h>
#include <stdio.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "init.h"
#include "font.h"
#include "game.h"
#include "mymath.h"
#include "soundmusic.h"
#include "texture.h"
#include "hiscore.h"
#include "menu.h"
#include "timer.h"
#include "screenshot.h"

#ifndef WIN32
#include <string>
using namespace std;
#endif


// Hiscore lists
HISCORE_LIST hiscore_1;
HISCORE_LIST hiscore_2;


// Fading stuff (from game.cpp)
// Are we fading? (0 == no, 1 == fade in, 2 == fade out)
extern int fading;
extern float fade_amount;


#ifndef WIN32
// Helper function from config.cpp
string get_tomatoes_dir();
#endif

// Helper function which returns suitable path for the
// hiscore file. It first checks the user's home directory,
// and if that fails it uses the HISCORE_DIR defined in the
// makefile.
char *get_hiscore_location(int which, bool write) {
#ifndef WIN32
	// Get the path to the hiscore file
	string tmp;
	if(which == 1)
		tmp = get_tomatoes_dir() + HISCORE_FILE;
	else
		tmp = get_tomatoes_dir() + HISCORE_FILE2;

	// Check if the hiscore file exists there
	if(!write) {
		FILE *ftest = fopen(tmp.c_str(), "rb");
		if(!ftest) {
			// It doesn't exist, try the default
			if(which == 1)
				return (HISCORE_DIR HISCORE_FILE);
			else
				return (HISCORE_DIR HISCORE_FILE2);
		}
		fclose(ftest);
	}

	return (char*)tmp.c_str();
#endif

	// Return the HISCORE_DIR
	if(which == 1)
		return (char*)(HISCORE_DIR HISCORE_FILE);
	else
		return (char*)(HISCORE_DIR HISCORE_FILE2);
}


// From game.cpp
bool handle_event(SDL_Event &event);

// Input name and update the list
void HISCORE_LIST::input_name(int place) {

	// We modify directly the list's names[place] entry
	memset(list[place].name, 0, sizeof(list[place].name));
	char *str = list[place].name;

	// Current position in string
	int pos = 0;
	bool typed = false;

	int cursor_blink = 0;
	back_anim = 0.0f;
	bx_roll = RANDF(0,1);
	by_roll = RANDF(0,1);
	bx_roll_dir = RANDF(-0.001f,0.001f);
	by_roll_dir = RANDF(-0.001f,0.001f);
	memset(key, 0, sizeof(key));

	// Fade in
	fading = 1;
	fade_amount = 1.0f;


	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// Loop
	timer_count = 0;
	bool done = false;
	while(!done) {
		while(timer_count > 0) {
			// Handle events
			SDL_Event event;
			while(SDL_PollEvent(&event)) {
				done = handle_event(event);		// from 'game.cpp'
			}

			// If the current music has finished, start playing another
			check_music_finished();

			// Fade
			if(fading) {
				if(fading == 1) {
					// Fade in
					fade_amount -= 0.01f;
					if(fade_amount <= 0.0f) {
						fading = 0;
						fade_amount = 0.0f;
					}
				}
				else if(fading == 2) {
					// Fade out
					fade_amount += 0.01f;
					if(fade_amount >= 1.0f) {
						fading = 0;
						fade_amount = 1.0f;
						done = true;
					}
				}
			}

			// Take a screenshot?
			if(key[SDLK_F12])
				save_screenshot();

			// Blink
			cursor_blink++;
			cursor_blink %= 20;

			// Get the keyboard modifiers
			SDLMod keymods = SDL_GetModState();

			// Check input
			typed = true;
			if(key[SDLK_0])
				str[pos] = '0';
			else if(key[SDLK_EXCLAIM] || ((keymods & KMOD_SHIFT) && key[SDLK_1]))
				str[pos] = '!';
			else if(key[SDLK_1])
				str[pos] = '1';
			else if(key[SDLK_2])
				str[pos] = '2';
			else if(key[SDLK_3])
				str[pos] = '3';
			else if(key[SDLK_4])
				str[pos] = '4';
			else if(key[SDLK_5])
				str[pos] = '5';
			else if(key[SDLK_6])
				str[pos] = '6';
			else if(key[SDLK_7])
				str[pos] = '7';
			else if(key[SDLK_8])
				str[pos] = '8';
			else if(key[SDLK_9])
				str[pos] = '9';
			else if(key[SDLK_a])
				str[pos] = 'A';
			else if(key[SDLK_b])
				str[pos] = 'B';
			else if(key[SDLK_c])
				str[pos] = 'C';
			else if(key[SDLK_d])
				str[pos] = 'D';
			else if(key[SDLK_e])
				str[pos] = 'E';
			else if(key[SDLK_f])
				str[pos] = 'F';
			else if(key[SDLK_g])
				str[pos] = 'G';
			else if(key[SDLK_h])
				str[pos] = 'H';
			else if(key[SDLK_i])
				str[pos] = 'I';
			else if(key[SDLK_j])
				str[pos] = 'J';
			else if(key[SDLK_k])
				str[pos] = 'K';
			else if(key[SDLK_l])
				str[pos] = 'L';
			else if(key[SDLK_m])
				str[pos] = 'M';
			else if(key[SDLK_n])
				str[pos] = 'N';
			else if(key[SDLK_o])
				str[pos] = 'O';
			else if(key[SDLK_p])
				str[pos] = 'P';
			else if(key[SDLK_q])
				str[pos] = 'Q';
			else if(key[SDLK_r])
				str[pos] = 'R';
			else if(key[SDLK_s])
				str[pos] = 'S';
			else if(key[SDLK_t])
				str[pos] = 'T';
			else if(key[SDLK_u])
				str[pos] = 'U';
			else if(key[SDLK_v])
				str[pos] = 'V';
			else if(key[SDLK_w])
				str[pos] = 'W';
			else if(key[SDLK_x])
				str[pos] = 'X';
			else if(key[SDLK_y])
				str[pos] = 'Y';
			else if(key[SDLK_z])
				str[pos] = 'Z';
			else if(key[SDLK_COMMA])
				str[pos] = ',';
			else if(key[SDLK_PERIOD])
				str[pos] = '.';
			else if(key[SDLK_KP_MINUS] || key[SDLK_MINUS])
				str[pos] = '-';
			else if(key[SDLK_SPACE])
				str[pos] = ' ';
			else if(key[SDLK_BACKSPACE]) {
				play_sound(SND_MENU1, false);
				typed = false;
				str[pos] = '\0';
				pos--;
				if(pos < 0)
					pos = 0;
				str[pos] = '\0';
			}
			else if(key[SDLK_RETURN] && pos > 0 && str[0] != ' ') {
				fading = 2;
				typed = false;
			}
			else {
				typed = false;
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


			// Modify string
			if(typed) {
				play_sound(SND_MENU1, false);
				pos++;
				if(pos > NAME_LEN-1)
					pos = NAME_LEN-1;
				str[pos] = '\0';
				typed = false;
			}

			timer_count--;
		}
		// Draw the menu (with the list)
		draw_menu(-1, -1, place, fade_amount, this);

		begin_fonts();

		// Draw the cursor
		float c = 1 - fade_amount;
		if(cursor_blink < 10 && fading != 2 && !done) {
			glLoadIdentity();
			set_font_scale(0.6f, 0.6f);
			glTranslatef(-3.7f + glfont_length(str) + 0.15f, 2.2f - place * 0.7f, -13);
			glColor3f(c,c,c);
			glBegin(GL_TRIANGLE_STRIP);
				glVertex2f(0.3f, 0.6f);
				glVertex2f(0.0f, 0.6f);
				glVertex2f(0.3f, 0.0f);
				glVertex2f(0.0f, 0.0f);
			glEnd();
		}

		// Draw the congratulations message
		set_font_scale(1,1);
		glLoadIdentity();
		glColor3f(c,c,c);
		glEnable(GL_BLEND);
		glprintf_center(font1, 0, 0, -4.5f, -13, (char*)"CONGRATULATIONS");

		end_fonts();


		// Flush and swap the buffers
		glFlush();
		SDL_GL_SwapBuffers();
	}

	SDL_EnableKeyRepeat(0,0);
}


// Draw the list. Hilights the 'place'.
void HISCORE_LIST::draw(int place, float fade) {
	float ypos = 2.2f;

	set_font_scale(0.8f,0.7f);
	glColor4f(0.1f,1,1, fade);
	glprintf_center(font1, 0, 0, 3.05f, -13, (char*)"%s", ((this == &hiscore_1) ? (char*)"SINGLE PLAYER HIGH SCORES" : (char*)"TWO PLAYERS HIGH SCORES"));

	set_font_scale(0.6f, 0.6f);

	for(int f=0; f<NUM_NAMES; f++) {
		float a = (float)f / (float)NUM_NAMES;
		a = 1.0f - (a * 0.4f);

		// Draw the number
		if(place != f)
			glColor4f(a,a,0.8f*a, fade);
		else
			glColor4f(1,1,0.8f, fade);
		glprintf(font1, 0, -5.0f, ypos, -13, (char*)"%2d.", f+1);

		// Draw the name
		if(place != f)
			glColor4f(a,a,a, fade);
		else
			glColor4f(1,1,1, fade);
		glprintf(font1, 0, -3.7f, ypos, -13, (char*)"%s", list[f].name);

		// Draw the score
		if(place != f)
			glColor4f(0.8f*a,a,0.8f*a, fade);
		else
			glColor4f(0.8f,1,0.8f, fade);
		glprintf(font1, 0, 3.4f, ypos, -13, (char*)"%03d", list[f].score);

		// Draw the hilight bar
		if(place == f) {
			glColor4f(0.2f, 0.2f, 0.2f, fade);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			BIND_TEXTURE(0);
			glBegin(GL_TRIANGLE_STRIP);
				glVertex3f(4.7f, ypos + 0.65f, -13);
				glVertex3f(-4.7f, ypos + 0.65f, -13);
				glVertex3f(4.7f, ypos - 0.05f, -13);
				glVertex3f(-4.7f, ypos - 0.05f, -13);
			glEnd();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		ypos -= 0.7f;
	}

	set_font_scale(1,1);
}


// Save the list to a file
void HISCORE_LIST::save(char *file) {
	FILE *fout;
	fout = fopen(file, "wb");
	if(!fout)
		error_msg("Unable to save the hiscore list to %s!\n", file);

	// Write the scores
	int f;
	for(f=0; f<NUM_NAMES; f++)
		fwrite(&list[f].score, sizeof(int), 1, fout);

	// Write the names
	for(f=0; f<NUM_NAMES; f++)
		fwrite(list[f].name, NAME_LEN, 1, fout);

	fclose(fout);
}


// Load the list from a file
void HISCORE_LIST::load(char *file) {
	FILE *fin;
	fin = fopen(file, "rb");
	if(!fin) {
		// The list wasn't found, no problem.
		clear();
		return;
	}

	// Read the scores
	int f;
	for(f=0; f<NUM_NAMES; f++)
		fread(&list[f].score, sizeof(int), 1, fin);

	// Read the names
	for(f=0; f<NUM_NAMES; f++)
		fread(list[f].name, NAME_LEN, 1, fin);

	fclose(fin);
}


// Add a name to the list. Returns the place.
int HISCORE_LIST::add_name(char *name, int score) {
	// Check if we qualify
	if(list[NUM_NAMES-1].score >= score)
		return -1;

	// Replace the last entry
	memset(list[NUM_NAMES-1].name, 0, sizeof(list[NUM_NAMES-1].name));
	strcpy(list[NUM_NAMES-1].name, name);
	list[NUM_NAMES-1].score = score;

	// Sort
	sort();

	// Find ourselves and return the place
	for(int f=NUM_NAMES-1; f >= 0; f--)
		if(strcmp(name, list[f].name) == 0 && score == list[f].score)
			return f;

	// This shouldn't happen..
	return -1;
}


// Comparison function for qsort
int sort_cmp(const void *a, const void *b) {
	RECORD ra = *(RECORD*)a;
	RECORD rb = *(RECORD*)b;

	// Sort by the score
	if(ra.score < rb.score)
		return 1;
	else if(ra.score > rb.score)
		return -1;

	// The scores are same, sort by the name
	return strcmp(ra.name, rb.name);
}


// Sort the list using qsort
void HISCORE_LIST::sort() {
	qsort(list, NUM_NAMES, sizeof(RECORD), sort_cmp);
}


// Clear the list
void HISCORE_LIST::clear() {
	for(int f=0; f<NUM_NAMES; f++) {
		memset(list[f].name, 0, sizeof(list[f].name));
		strcpy(list[f].name, "- - - - - - - - - -");
		list[f].score = 0;
	}
}
