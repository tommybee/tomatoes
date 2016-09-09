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

#ifndef FONT_H
#define FONT_H

// Implements basic bitmapped fonts
// Based on tutorial by Jeff "Nehe" Molofee

// Display list for bitmapped fonts
extern GLuint font_base;
extern GLuint font_num_base;

// Font scale
extern float font_scale_x, font_scale_y;

// Font textures
extern GLuint font1;
extern GLuint font_num;


// Set the scale for the fonts
#define set_font_scale(a,b)		{ font_scale_x = a; font_scale_y = b; }

// Begin font drawing
#define begin_fonts()	{	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); \
							glDisable(GL_DEPTH_TEST); \
							glMatrixMode(GL_PROJECTION); \
							glLoadIdentity(); \
							gluPerspective(45.0f, 1.333333f, 1, 100); \
							glMatrixMode(GL_MODELVIEW); }


// End font drawing
#define end_fonts()		{	glDisable(GL_BLEND); \
							glEnable(GL_DEPTH_TEST); }


// Delete the font display list
#define delete_font_list()		{ glDeleteLists(font_base, 256); glDeleteLists(font_num_base, 10); }



// Print a string with specified font texture
void glprintf(GLuint tex, int set, float x, float y, float z, char *string, ...);


// Print a centered string with specified font texture
void glprintf_center(GLuint tex, int set, float x, float y, float z, char *string, ...);


// Return the length of a string
float glfont_length(char *string, ...);


// Print numbers using the big number font
void glprint_num(float x, float y, float z, char *string);


// Build a display list for bitmapped fonts (-> font_base)
void build_font_list();



#endif

