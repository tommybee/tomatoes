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

#ifndef MENU_H
#define MENU_H

#include "hiscore.h"

// Menu IDs
#define MENU_ID_MAIN				0
	#define MENU_START				1
	#define MENU_OPTIONS			2
	#define MENU_EXIT				3
#define MENU_ID_START				4
	#define MENU_SINGLEPLAY			5
	#define MENU_MULTIPLAY			6
#define MENU_ID_OPTIONS				7
	#define MENU_WHOSEKEYS			8
	#define MENU_MOVSTYLE			9
	#define MENU_KEYUP				10
	#define MENU_KEYDOWN			11
	#define MENU_KEYLEFT			12
	#define MENU_KEYRIGHT			13
	#define MENU_KEYBOMB			14
	#define MENU_KEYSPECIAL			15
	#define MENU_SOUNDVOL			16
	#define MENU_MUSICVOL			17
	#define MENU_PERSPECTIVE		18
	#define MENU_OK					19
	#define MENU_CANCEL				20


// Background animation
extern float back_anim;
extern float bx_roll, by_roll;
extern float bx_roll_dir, by_roll_dir;

void load_menus();
int show_menu(int menu_id);
void draw_menu(int menu_id, int menu_item, int place, float fade, HISCORE_LIST *list);

#endif
