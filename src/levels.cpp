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
#include <vector>
#include "SDL.h"
#include "SDL_opengl.h"
#include "mpak.h"
#include "levels.h"
#include "enemy.h"
#include "particle.h"
#include "bonus.h"
#include "bomb.h"
#include "tilemap.h"
#include "timer.h"
#include "special_power.h"
#include "trap.h"
#include "game.h"
#include "bgrounds.h"
#include "mymath.h"
#include "helpers.h"
#include "select_special.h"
#include "comments.h"
#include "soundmusic.h"
#include "texture.h"


using namespace std;

// Number of levels per game
#define NUM_LEVELS			10

// Level list
int level_list[NUM_LEVELS];

// Total number of level files
int num_level_files;

// Level names
static char level_names[NUM_LEVELS][16] = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten" };


// Current level
int cur_level;

// Level time counter
Uint32 level_time;

// Number of seconds left
int num_seconds_left;
static Uint32 sec_counter;


// Level change interval
const Uint32 level_change_interval = 60 * 1000;


// Is the level begin/end pause active?
int level_pause = 0;



// Return the number of seconds left
float get_level_seconds_left() {
	return ((float)level_change_interval/1000.0f - ((float)(SDL_GetTicks() - level_time) / 1000.0f));
}



// Find the levels
void find_levels() {
	// Find the levels from the pakfile
	num_level_files = 0;
	while(1) {
		// Check if the file exists
		char str[128] = "";
		sprintf(str, "MAP%02d.map", num_level_files+1);

		FILE *check = pakfile.open_file(str);
		if(!check)
			// No more levels, break
			break;
		else {
			// There was a level by that name.
			fclose(check);
			num_level_files++;
		}
	}
}


// Shuffle the level list
void shuffle_level_list() {
	// Shuffle the level files and take NUM_LEVELS amount of levels into the
	// level_list array
	vector<int> shuffled;
	shuffled.clear();
	for(int f=0; f<num_level_files; f++)
		shuffled.push_back(f);

	// Shuffle it
	for(unsigned int i = shuffled.size() - 1; i > 0; i--) {
		int pos = RAND(0, i - 1);
		int swap = shuffled[i];
		shuffled[i] = shuffled[pos];
		shuffled[pos] = swap;
	}

	// Take the first NUM_LEVELS levels
	for(int j=0; j<NUM_LEVELS; j++)
		level_list[j] = shuffled[j];

}


// Shuffle the background list
void shuffle_bground_list() {
	vector<int> shuffled;
	shuffled.clear();
	for(int f=0; f<NUM_BGROUNDS; f++)
		shuffled.push_back(f);

	// Shuffle it
	for(unsigned int i = shuffled.size() - 1; i > 0; i--) {
		int pos = RAND(0, i - 1);
		int swap = shuffled[i];
		shuffled[i] = shuffled[pos];
		shuffled[pos] = swap;
	}

	// Copy the list
	for(int j=0; j<NUM_BGROUNDS; j++)
		bgrounds[j] = shuffled[j];
}


// Load map by number
void load_map_number(int num) {
	char str[128] = "";
	sprintf(str, "MAP%02d.map", num+1);
	load_map(str);
}


// Load bground by number
void load_bg_number(int num) {
	char str[128] = "";
	sprintf(str, "bg%02d.jpg", num+1);

	// Delete the existing texture and load over it
	glDeleteTextures(1, &cur_bground);
	glGenTextures(1, &cur_bground);

	// Load the background image
	load_texture_into(cur_bground, str, "JPG", false, false, true);
}


// Init the levels
void init_levels() {
	cur_level = 0;
	using_special_power = 0;
	special_power_pause = false;
	init_special_selection();

	// Shuffle the level list and load the first map
	shuffle_level_list();
	load_map_number(level_list[0]);
	level_pause = LEVEL_PAUSE_BEGIN;

	// Shuffle the background list and load the first background
	shuffle_bground_list();
	load_bg_number(bgrounds[0]);

	// Make the icons visible
	if(two_players) {
		show_icon(0);
		show_icon(1);
	}


	// Add the comment
	add_comment(COL_DEFAULT, "Press ENTER to begin level %s.", level_names[cur_level]);

	// Clear the helpers
	wisp.clear();
	potatoman.clear();


	// Set the bonus system
	for(int f=0; f<NUM_ICONS; f++)
		icon_menu.count[f] = 0;
	next_bonus[BONUS_RED] = RED_POWER_TRAP;
	next_bonus[BONUS_GREEN] = GREEN_POWER_WISP;
	next_bonus[BONUS_BLUE] = BLUE_POWER_TELEPORT;
	bonus_counter[BONUS_RED] = bonus_counter[BONUS_GREEN] = bonus_counter[BONUS_BLUE] = 0;

	num_seconds_left = level_change_interval / 1000;
}


// Helper function from player.cpp
void get_random_block_at(int x, int y, int &bx, int &by);


static bool sound_played = false;
static bool comment_added = false;

// Change level.
// Return false if we're finished all the levels.
bool change_level() {
	if(level_pause == LEVEL_PAUSE_END) {
		level_time--;

		if(level_time <= 0) {
			// Switch to the next level
			if(level_pause == LEVEL_PAUSE_END) {
				// Check if we're finished all the levels.
				if(cur_level == NUM_LEVELS-1)
					return false;
				next_level();
			}
		}
		return true;
	}

	// Check if there's ten seconds left
	num_seconds_left = (int)get_level_seconds_left();
	if(num_seconds_left == 10 && !comment_added) {
		add_comment(COL_DEFAULT, "Ten seconds left.");
		comment_added = true;
	}

	// Check if there's three seconds left
	if(num_seconds_left == 3 && !sound_played) {
		play_sound(SND_FINISH, false);
		sound_played = true;
	}


	// Advance the level time
	Uint32 time = SDL_GetTicks() - level_time;
	if(time >= level_change_interval) {
		// Level is finished
		level_time = 3 * 60;

		// Jump the players to the blocks
		int bx, by;
		if(p1.alive && !p1.dying && !map_solid((int)p1.get_real_x(), (int)p1.get_real_y()) && !p1.jumping && !(using_special_power == 1 && which_special_power == BLUE_POWER_TELEPORT)) {
			get_random_block_at((int)p1.get_real_x(), (int)p1.get_real_y(), bx, by);
			p1.jump(bx, by, 2.0f, 0.05f);

			// Play the jump sound
			play_sound(SND_JUMP, false);
		}

		if(two_players && p2.alive && !p2.dying && !map_solid((int)p2.get_real_x(), (int)p2.get_real_y()) && !p2.jumping && !(using_special_power == 2 && which_special_power == BLUE_POWER_TELEPORT)) {
			get_random_block_at((int)p2.get_real_x(), (int)p2.get_real_y(), bx, by);
			p2.jump(bx, by, 2.0f, 0.05f);

			// Play the jump sound
			play_sound(SND_JUMP, false);
		}


		// Explode the player bombs
		if(p1.num_bombs > 0) {
			list<BOMB>::iterator b;
			for(b = bomblist.begin(); b != bomblist.end(); ++b)
				if((*b).owner == 1 && (*b).time > 20)
					(*b).time = 20;		// Makes the bomb explode soon
		}

		if(two_players && p2.num_bombs > 0) {
			list<BOMB>::iterator b;
			for(b = bomblist.begin(); b != bomblist.end(); ++b)
				if((*b).owner == 2 && (*b).time > 20)
					(*b).time = 20;		// Makes the bomb explode soon
		}

		// Kill all the burning enemies
		if(enemylist.size() > 0) {
			list<ENEMY>::iterator e;
			for(e = enemylist.begin(); e != enemylist.end(); ++e) {
				if((*e).burning)
					(*e).die();
			}
		}

		// Add the comment
		level_pause = LEVEL_PAUSE_END;
		add_comment(COL_DEFAULT, "Level finished.");
		if(cur_level == NUM_LEVELS-1)
			add_comment(COL_DEFAULT, "GAME OVER");
	}

	return true;
}


// Switch to the next level
void next_level() {
	// Proceed to the next level
	cur_level++;
	if(cur_level >= NUM_LEVELS) {
		// We have finished the game.
		return;
	}
	num_seconds_left = level_change_interval / 1000;

	game_paused = true;

	// Clear stuff
	p1.clear();
	p2.clear();
	clear_enemies();
	clear_bombs();
	clear_particles();
	clear_bonus();
	clear_traps();
	using_special_power = 0;
	special_power_pause = false;
	init_special_selection();

	// Make the icons visible
	if(two_players) {
		show_icon(0);
		show_icon(1);
	}

	// Load the level
	load_map_number(level_list[cur_level]);

	// Load the background
	load_bg_number(bgrounds[cur_level]);

	// Randomize the location for the wisps and the potato men
	if(wisp.alive) {
		wisp.pos.x = RANDF(0, MAP_W);
		wisp.pos.y = 5.0f;
		wisp.pos.z = RANDF(0, MAP_H);
		wisp.dir = 0.0f;
		wisp.bonus = NULL;
	}
	if(potatoman.alive) {
		int old_counter = potatoman.counter;
		create_potatoman();
		potatoman.counter = old_counter;
	}

	// Add the comment
	add_comment(COL_DEFAULT, "Press ENTER to begin level %s.", level_names[cur_level]);
	level_pause = LEVEL_PAUSE_BEGIN;

	start_level_timing();
	comment_added = false;
	sound_played = false;
	game_paused = false;
}


// Start timing the level change
void start_level_timing() {
	level_time = SDL_GetTicks();
	sec_counter = level_time;
}
