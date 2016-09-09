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
#include <stdarg.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "init.h"
#include "font.h"
#include "comments.h"

// Colors
const float comment_colors[5][3] = {
	{1,1,1}, {1,.7f,.7f}, {.7f,1,.7f}, {.7f,.7f,1}, {1,1,.7f}
};

// Comment position
#define COMMENT_POS		-13.6f


// Comments array
COMMENT comments[NUM_COMMENTS];


// Add a comment
void add_comment(int color, const char *str, ...) {
	// Format the string arguments
	char buf[256];
	va_list args;
	va_start(args, str);
	vsprintf(buf, str, args);
	va_end(args);

	// Move the comments down
	for(int f=NUM_COMMENTS-1; f >= 1; f--)
		comments[f].copy(comments[f-1]);

	// Add the new comment
	comments[0].set(color, buf);
}


// Move the comments
void move_comments() {
	for(int f=0; f<NUM_COMMENTS; f++)
		if(comments[f].active)
			comments[f].move();
}


// Draw the comments
void draw_comments() {
	float ypos = -7.0f;

	for(int f=0; f<NUM_COMMENTS; f++) {
		if(comments[f].active) {
			comments[f].draw(ypos);
			ypos -= 1.0f;
		}
	}
}


// Clear the comments
void clear_comments() {
	for(int f=0; f<NUM_COMMENTS; f++)
		comments[f].clear();
}



// Set the text
void COMMENT::set(int color_, char *str) {
	clear();

	// Set the buf
	strcpy(text, str);

	color = color_;
	active = true;
}


// Copy the comment
void COMMENT::copy(COMMENT &from) {
	strcpy(text, from.text);
	active = from.active;
	color = from.color;
	alpha = from.alpha;
	pos = from.pos;
}


// Draw the comment
void COMMENT::draw(float ypos) {
	// Set the color and draw the string
	glColor4f(comment_colors[color][0],comment_colors[color][1],comment_colors[color][2], alpha);
	glprintf(font1, 0, pos, ypos, -25, text);
}



// Move the comment
void COMMENT::move() {
	// Move the comment
	if(pos > COMMENT_POS) {
		float dist = pos - COMMENT_POS;
		pos -= 1.5f * (dist / (-COMMENT_POS+14.0f));
		if(pos < COMMENT_POS)
			pos = COMMENT_POS;
	}

	// Fade the text
	alpha -= 0.003f;
	if(alpha < 0.0f)
		active = false;
}


// Clear the comment
void COMMENT::clear() {
	memset(text, 0, sizeof(text));
	color = COL_DEFAULT;
	alpha = 1.0f;
	pos = 14.0f;
	active = false;
}

