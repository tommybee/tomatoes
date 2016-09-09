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
#include <stdarg.h>
#include <string.h>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"
#include "font.h"
#include "texture.h"


// Spacing of the fonts
const float spacing = 0.55f;

// Font scale
float font_scale_x = 1.0f, font_scale_y = 1.0f;

// Display list for bitmapped fonts
GLuint font_base = 0;
GLuint font_num_base = 0;

// Font textures
GLuint font1;
GLuint font_num;


// Return the length of a string
float glfont_length(char *string, ...) {

	// Format the string arguments
	char buf[1024];
	va_list args;
	va_start(args, string);
	vsprintf(buf, string, args);
	va_end(args);

	float length = (float)strlen(buf)*spacing;
	if(font_scale_x != 1.0f)
		length *= font_scale_x;

	return length;
}


// Print a string with specified font texture
void glprintf(GLuint tex, int set, float x, float y, float z, char *string, ...) {

	// Format the string arguments
	char buf[1024];
	va_list args;
	va_start(args, string);
	vsprintf(buf, string, args);
	va_end(args);

	// Print
	BIND_TEXTURE(tex);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x,y,z);

	if(font_scale_x != 1.0f || font_scale_y != 1.0f)
		glScalef(font_scale_x, font_scale_y, 1);

	glListBase(font_base - 32 + (128*set));
	glCallLists(strlen(buf), GL_BYTE, buf);

	glPopMatrix();
}


// Print a centered string with specified font texture
void glprintf_center(GLuint tex, int set, float x, float y, float z, char *string, ...) {

	// Format the string arguments
	char buf[1024];
	va_list args;
	va_start(args, string);
	vsprintf(buf, string, args);
	va_end(args);

	// Print
	BIND_TEXTURE(tex);
	glPushMatrix();
	glLoadIdentity();
	float length = (float)strlen(buf)*spacing;
	if(font_scale_x != 1.0f)
		length *= font_scale_x;
	glTranslatef(x-(length * 0.5f),y,z);

	if(font_scale_x != 1.0f || font_scale_y != 1.0f)
		glScalef(font_scale_x, font_scale_y, 1);

	glListBase(font_base - 32 + (128*set));
	glCallLists(strlen(buf), GL_BYTE, buf);

	glPopMatrix();
}


// Build a display list for bitmapped fonts (-> font_base)
void build_font_list() {

	float cx, cy;

	font_base = glGenLists(256);
	BIND_TEXTURE(0);

	// For normal fonts
	int f;
	for(f=0; f<256; f++) {
		cx = float(f%16)/16.0f;
		cy = float(f/16)/16.0f;

		glNewList(font_base+f, GL_COMPILE);
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(cx+0.0625f, 1-cy); glVertex2f(1,1);
				glTexCoord2f(cx, 1-cy); glVertex2f(0,1);
				glTexCoord2f(cx+0.0625f, 1-cy-0.0625f); glVertex2f(1,0);
				glTexCoord2f(cx, 1-cy-0.0625f); glVertex2f(0,0);
			glEnd();
			glTranslatef(spacing, 0,0);
		glEndList();
	}

	// For the big numbers
	font_num_base = glGenLists(10);
	int xx = -1, yy = 0;
	for(f=0; f<10; f++) {
		xx++;
		if(xx > 7) {
			xx = 0;
			yy++;
		}

		cx = (float)xx / 8.0f;
		cy = (float)yy / 2.0f;

		glNewList(font_num_base+f, GL_COMPILE);
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(cx+0.125f, 1-cy); glVertex2f(1,1);
				glTexCoord2f(cx, 1-cy); glVertex2f(0,1);
				glTexCoord2f(cx+0.125f, 1-cy-0.5f); glVertex2f(1,0);
				glTexCoord2f(cx, 1-cy-0.5f); glVertex2f(0,0);
			glEnd();
			glTranslatef(0.8f, 0,0);
		glEndList();
	}

}



// Print numbers using the big number font
void glprint_num(float x, float y, float z, char *string) {

	// Print
	BIND_TEXTURE(font_num);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(x,y,z);

	if(font_scale_x != 1.0f || font_scale_y != 1.0f)
		glScalef(font_scale_x, font_scale_y, 1);

	int len = strlen(string);

	for(int f=0; f<len; f++) {
		int c = (int)string[f];
		if(c == 32) {
			// It's a space, skip it
			glTranslatef(0.8f, 0,0);
			continue;
		}

		// Check for numbers
		if(c >= 48 && c <= 57) {
			// Draw the number
			c -= 48;
			glCallList(c + font_num_base);
		}
	}

	glPopMatrix();
}


