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

#ifndef CONFIG_H
#define CONFIG_H

// Config structure
struct CONFIG {
	int vid_w;				// Video mode width
	int vid_h;				// Video mode height
	int vid_color_depth;	// Video mode color depth
	int fullscreen;			// Fullscreen (1) or window (0)
	int sound;				// Sound enabled (1) or disabled (0)
	int sound_freq;			// Sound frequency
	int sound_vol;			// Sound volume (0-255)
	int music_vol;			// Music volume (0-255)
	int show_fps;			// Show fps amount?

	// Keys
	int key_left[2];
	int key_right[2];
	int key_up[2];
	int key_down[2];
	int key_shoot[2];
	int key_special[2];

	int perspective_mode;	// Perspective mode enabled (1) or disabled (0)
	int moving_style[2];	// Moving style (1 == relative, 2 == absolute)
};


// Helper function which returns suitable path for the
// config file. It first checks the user's home directory,
// and if that fails it uses the CONFIG_DIR defined in the
// makefile.
char *get_config_location(bool write = false);


// Load config from file
void load_config(char *file, CONFIG *conf);

// Save config to file
void save_config(char *file, CONFIG *conf);

#endif


