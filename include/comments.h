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

#ifndef COMMENTS_H
#define COMMENTS_H


// Comment colors
#define COL_DEFAULT			0
#define COL_RED				1
#define COL_GREEN			2
#define COL_BLUE			3
#define COL_YELLOW			4

// Number of comments on screen
#define NUM_COMMENTS		4


// Comment class
class COMMENT {
public:
	char text[256];						// Comment text
	int color;							// Comment color
	float alpha;						// Comment alpha
	float pos;							// Comment position

	bool active;						// Is the comment active?

	// Functions
	void clear();						// Clear the comment
	void move();						// Move the comment
	void draw(float ypos);				// Draw the comment
	void set(int color, char *str);		// Set the comment
	void copy(COMMENT &from);			// Copy the comment
};


void add_comment(int color, const char *str, ...);
void draw_comments();
void move_comments();
void clear_comments();

#endif
