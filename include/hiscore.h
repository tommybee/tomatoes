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

#ifndef HISCORE_H
#define HISCORE_H

// Number of names in the hiscore list
#define NUM_NAMES			8

// Name length
#define NAME_LEN			20

// Hiscore file
#define HISCORE_FILE		"hiscore.lst"
#define HISCORE_FILE2		"hiscore2.lst"


// Record structure
struct RECORD {
	char name[NAME_LEN];			// Name
	int score;						// Score, i.e. number of kills
};


// Hiscore list
class HISCORE_LIST {
public:
	RECORD list[NUM_NAMES];						// Records

	void clear();								// Clear the list
	void sort();								// Sort the list
	int add_name(char *name, int score);		// Add a record
	void draw(int place, float fade);			// Draw the list
	void save(char *file);						// Save the list
	void load(char *file);						// Load the list
	void input_name(int place);					// Input a name
};

// Hiscore lists
extern HISCORE_LIST hiscore_1;
extern HISCORE_LIST hiscore_2;

char *get_hiscore_location(int which, bool write = false);


#endif
