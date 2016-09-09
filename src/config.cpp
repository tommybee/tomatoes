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
#include "config.h"
#include "init.h"


// The config files are saved to the users HOME directory in Linux, but
// not in Windows.
#ifndef WIN32
#include <string>
#include <sys/stat.h>
#include <dirent.h>
using namespace std;


// Helper function which makes sure that .tomatoes directory
// exists in the user's home directory.
string get_tomatoes_dir() {
	// Get the HOME environment variable
	char *home = getenv("HOME");
	if(home != NULL) {
		string tmp = (string)home + "/.tomatoes/";
		// Check if the directory exists
		DIR *dp = NULL;
		dp = opendir(tmp.c_str());
		if(!dp) {
			// It doesn't exist, try to create it
			if(mkdir(tmp.c_str(), S_IRWXU))
				// Can't create it
				return "";
			return tmp;
		}
		else {
			// It does exist.
			closedir(dp);
			return tmp;
		}
	}

	// The HOME variable wasn't available
	return "";
}
#endif


// Helper function which returns suitable path for the
// config file. It first checks the user's home directory,
// and if that fails it uses the CONFIG_DIR defined in the
// makefile.
char *get_config_location(bool write) {
#ifndef WIN32
	// Get the path to the config file
	string tmp = get_tomatoes_dir() + "config.cfg";

	// Check if the config file exists there
	if(!write) {
		FILE *ftest = fopen(tmp.c_str(), "rt");
		if(!ftest) {
			// It doesn't exist, try the default
			return (CONFIG_DIR "config.cfg");
		}
		fclose(ftest);
	}

	return (char*)tmp.c_str();
#endif

	// Return the CONFIG_DIR
	return (char*)(CONFIG_DIR "config.cfg");
}


// Load config from file
void load_config(char *file, CONFIG *conf) {

	FILE *f = fopen(file, "rt");
	if(!f)
		error_msg("Unable to load config file: %s!", file);

	fscanf(f, "video_mode = %d x %d\n", &(conf->vid_w), &(conf->vid_h));
	fscanf(f, "video_mode_color_depth = %d\n", &(conf->vid_color_depth));
	fscanf(f, "video_mode_fullscreen = %d\n", &(conf->fullscreen));
	fscanf(f, "sound_enabled = %d\n", &(conf->sound));
	fscanf(f, "sound_freq = %d\n", &(conf->sound_freq));
	fscanf(f, "sound_volume = %d\n", &(conf->sound_vol));
	fscanf(f, "music_volume = %d\n", &(conf->music_vol));
	fscanf(f, "show_fps = %d\n", &(conf->show_fps));
	fscanf(f, "key_left = %d %d\n", &(conf->key_left[0]), &(conf->key_left[1]));
	fscanf(f, "key_right = %d %d\n", &(conf->key_right[0]), &(conf->key_right[1]));
	fscanf(f, "key_up = %d %d\n", &(conf->key_up[0]), &(conf->key_up[1]));
	fscanf(f, "key_down = %d %d\n", &(conf->key_down[0]), &(conf->key_down[1]));
	fscanf(f, "key_shoot = %d %d\n", &(conf->key_shoot[0]), &(conf->key_shoot[1]));
	fscanf(f, "key_special = %d %d\n", &(conf->key_special[0]), &(conf->key_special[1]));
	fscanf(f, "perspective = %d\n", &(conf->perspective_mode));
	fscanf(f, "moving_style = %d %d\n", &(conf->moving_style[0]), &(conf->moving_style[1]));
	fclose(f);
}


// Save config to file
void save_config(char *file, CONFIG *conf) {

	FILE *f = fopen(file, "wt");
	if(!f)
		error_msg("Unable to save config file: %s!", file);

	fprintf(f, "video_mode = %d x %d\n", (conf->vid_w), (conf->vid_h));
	fprintf(f, "video_mode_color_depth = %d\n", (conf->vid_color_depth));
	fprintf(f, "video_mode_fullscreen = %d\n", (conf->fullscreen));
	fprintf(f, "sound_enabled = %d\n", (conf->sound));
	fprintf(f, "sound_freq = %d\n", (conf->sound_freq));
	fprintf(f, "sound_volume = %d\n", (conf->sound_vol));
	fprintf(f, "music_volume = %d\n", (conf->music_vol));
	fprintf(f, "show_fps = %d\n", (conf->show_fps));
	fprintf(f, "key_left = %d %d\n", (conf->key_left[0]), (conf->key_left[1]));
	fprintf(f, "key_right = %d %d\n", (conf->key_right[0]), (conf->key_right[1]));
	fprintf(f, "key_up = %d %d\n", (conf->key_up[0]), (conf->key_up[1]));
	fprintf(f, "key_down = %d %d\n", (conf->key_down[0]), (conf->key_down[1]));
	fprintf(f, "key_shoot = %d %d\n", (conf->key_shoot[0]), (conf->key_shoot[1]));
	fprintf(f, "key_special = %d %d\n", (conf->key_special[0]), (conf->key_special[1]));
	fprintf(f, "perspective = %d\n", (conf->perspective_mode));
	fprintf(f, "moving_style = %d %d\n", (conf->moving_style[0]), (conf->moving_style[1]));
	fclose(f);
}


