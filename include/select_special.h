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

#ifndef SELECT_SPECIAL_H
#define SELECT_SPECIAL_H

// Number of special icons
#define NUM_ICONS			9


// This structure holds the icon menu data
struct ICON_MENU {
	int selected;							// Index of the selected icon
	float anim[NUM_ICONS];					// Animation states for each icon
	float pos;								// Position (between 0 and 1)

	int who;								// Who opened the menu?
	bool is_on_block;						// Is he/she standing on a block?

	int count[NUM_ICONS];					// How many bonuses we have?

	bool lkey_down, rkey_down;				// Are the keys down?
	int wait;								// Wait counter which waits a moment before opening the menu again
	bool visible;							// Is the menu visible?
};

extern ICON_MENU icon_menu;


void load_icons();
void init_special_selection();
void draw_special_selection();
void do_special_selection();
void open_icon_menu(int who, bool standing_on_block);


#endif
