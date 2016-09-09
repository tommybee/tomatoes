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
#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "init.h"
#include "timer.h"

#ifndef WIN32
#include <string>
using namespace std;

// From config.cpp
string get_tomatoes_dir();
#endif


// Save an array of pixels to a TGA file
void save_tga(char *filename, short int w, short int h, unsigned char *image_data);


// Check if a file exists
bool file_exists(char *file) {
	FILE *f = fopen(file, "rb");
	if(!f)
		return false;
	else {
		fclose(f);
		return true;
	}
}



// Save a screenshot
void save_screenshot() {

	char temp[512] = "";
	static int num = 0;
	unsigned char *image_data;

	game_paused = true;


	// Determine the filename
#ifndef WIN32
	sprintf(temp, "%sscreenshot%03d.tga", get_tomatoes_dir().c_str(), num);
	while(file_exists(temp)) {
		num++;
		sprintf(temp, "%sscreenshot%03d.tga", get_tomatoes_dir().c_str(), num);
	}
#else
	sprintf(temp, "%sscreenshot%03d.tga", CONFIG_DIR, num);
	while(file_exists(temp)) {
		num++;
		sprintf(temp, "%sscreenshot%03d.tga", CONFIG_DIR, num);
	}
#endif

	// Read the data
	image_data = (unsigned char*)malloc(screen->w * screen->h * 3);
	memset(image_data, 0, screen->w * screen->h * 3);
	glReadPixels(0, 0, screen->w, screen->h, GL_RGB, GL_UNSIGNED_BYTE, image_data);

	// Write the data
	save_tga(temp, screen->w, screen->h, image_data);

	// Free the data
	free(image_data);

	game_paused = false;
}



// Save an array of pixels to a TGA file
// Written using OpenGL Game Programming - book as a reference
void save_tga(char *filename, short int w, short int h, unsigned char *image_data) {

	unsigned char byte_skip;		// Used to fill in the data fields that we don't care about
	short int short_skip;
	unsigned char image_type;		// Type of image we're writing to the file
	int color_mode;
	unsigned char color_swap;
	int image_index;
	unsigned char bit_depth;
	long image_size;
	FILE *file;

	// Open the file in binary mode
	file = fopen(filename, "wb");
	if(!file)
		error_msg("Unable to save a screenshot to \"%s\"!\n", filename);

	image_type = 2;		// RGB, uncompressed
	bit_depth = 24;		// 24bpp
	color_mode = 3;		// RGB color mode

	byte_skip = 0;
	short_skip = 0;

	// Write 2 bytes of blank data
	fwrite(&byte_skip, sizeof(unsigned char), 1, file);
	fwrite(&byte_skip, sizeof(unsigned char), 1, file);

	// Write the image_type
	fwrite(&image_type, sizeof(unsigned char), 1, file);

	// Write more blank data
	fwrite(&short_skip, sizeof(short int), 1, file);
	fwrite(&short_skip, sizeof(short int), 1, file);
	fwrite(&byte_skip, sizeof(unsigned char), 1, file);
	fwrite(&short_skip, sizeof(short int), 1, file);
	fwrite(&short_skip, sizeof(short int), 1, file);

	// Write image dimensions
	fwrite(&w, sizeof(short int), 1, file);
	fwrite(&h, sizeof(short int), 1, file);
	fwrite(&bit_depth, sizeof(unsigned char), 1, file);

	// Write 1 byte of blank data
	fwrite(&byte_skip, sizeof(unsigned char), 1, file);

	// Compute the image size
	image_size = w * h * color_mode;

	// Change the image data from RGB to BGR
	for(image_index = 0; image_index < image_size; image_index += color_mode) {
		color_swap = image_data[image_index];
		image_data[image_index] = image_data[image_index + 2];
		image_data[image_index+2] = color_swap;
	}

	// Write the data
	fwrite(image_data, sizeof(unsigned char), image_size, file);

	// Close the file
	fclose(file);
}


