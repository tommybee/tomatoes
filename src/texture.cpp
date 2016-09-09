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

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_opengl.h"

#include "texture.h"
#include "init.h"
#include "mpak.h"

/*
// This gets a pixel from a surface
Uint32 getpixel(SDL_Surface *bmp, int x, int y) {

	int bpp = bmp->format->BytesPerPixel;
	Uint8 *p = (Uint8*)bmp->pixels + y * bmp->pitch + x*bpp;

	switch(bpp) {
		case 1:
			return *p;
		case 2:
			return *(Uint16*)p;
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		case 4:
			return *(Uint32*)p;

		default:
			return 0;
	}
}
*/


// Load a PNG
GLuint load_png(const char *file, bool alpha, bool repeat, bool mipmaps) {
	return load_texture(file, "PNG", alpha, repeat, mipmaps);
}


// Load a JPG
GLuint load_jpg(const char *file, bool alpha, bool repeat, bool mipmaps) {
	return load_texture(file, "JPG", alpha, repeat, mipmaps);
}


// Load a image file using SDL_Image and
// convert it to OpenGL texture.
// If alpha == true -> RGBA alpha texture
// If repeat == true -> texture can be tiled
// If mipmaps == true -> Mipmaps will be generated
// Return texture ID
GLuint load_texture(const char *ifile, const char *iimg_type, bool alpha, bool repeat, bool mipmaps) {

	GLuint tex;
	int width;
	// Load the 'file' to SDL_Surface
	SDL_Surface *img = NULL;
//	img = IMG_Load(file);
	
	char file[128];
	char img_type[128];
	
	memset(file, 0x00, sizeof(file));
	memset(img_type, 0x00, sizeof(img_type));
	
	memcpy(file, ifile, strlen(ifile)+1);
	memcpy(img_type, iimg_type, strlen(iimg_type)+1);
	
	FILE *fin = pakfile.open_file(file);
	if(!fin)
		error_msg("Unable to load texture from %s!\nSomething is wrong with the pakfile.", file);

	SDL_RWops *rw;
	rw = SDL_RWFromFP(fin, 1);
	img = IMG_LoadTyped_RW(rw,0, img_type);
	
	if(img == NULL)
		error_msg("Unable to load texture from %s!\n%s", file, IMG_GetError());
	SDL_FreeRW(rw);

	// Lock the surface
	if(SDL_MUSTLOCK(img))
		SDL_LockSurface(img);
	
	width = img->w;
	
	// Flip the surface pixels upside down
	Uint8 *line = new Uint8[width*img->format->BytesPerPixel];
	
	if(!line)
		error_msg("Unable to flip surface %s:\nOut of memory!\n");

	for(int f=0; f < img->h/2; f++) {
		memcpy((Uint8*)line, ((Uint8*)img->pixels + f*img->pitch), img->w*img->format->BytesPerPixel);
		memcpy(((Uint8*)img->pixels + f*img->pitch), ((Uint8*)img->pixels + (img->h - f - 1)*img->pitch), img->w*img->format->BytesPerPixel);
		memcpy(((Uint8*)img->pixels + (img->h - f - 1)*img->pitch), (Uint8*)line, img->w*img->format->BytesPerPixel);
	}

	delete [] line;

/*
	// Build the texture from the surface
	int dim = img->w * img->h * ((alpha) ? 4 : 3);
	GLubyte *data;
	data = new GLubyte[dim];
	if(!data)
		error_msg("Unable to create a texture from %s!", file);

	// Traverse trough surface and grab the pixels
	int pos = 0;
	for(int y=(img->h-1); y>-1; y--) {
		for(int x=0; x<img->w; x++) {
			Uint8 r,g,b,a;
			Uint32 color = getpixel(img, x,y);

			if(!alpha)
				SDL_GetRGB(color, img->format, &r,&g,&b);
			else
				SDL_GetRGBA(color, img->format, &r,&g,&b,&a);

			data[pos] = r; pos++;
			data[pos] = g; pos++;
			data[pos] = b; pos++;
			if(alpha) {
				data[pos] = a; pos++;
			}
		}
	}
*/

	// Generate the OpenGL texture ID
	int type = (alpha) ? GL_RGBA : GL_RGB;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	// Upload the texture data
	glTexImage2D(GL_TEXTURE_2D, 0, type, img->w, img->h, 0, type, GL_UNSIGNED_BYTE, img->pixels);
	//glTexImage2D(GL_TEXTURE_2D, 0, type, img->w, img->h, 0, type, GL_UNSIGNED_BYTE, data);


	// Set up mip mapping
	int filter_min = (mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
	int filter_mag = GL_LINEAR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (repeat) ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (repeat) ? GL_REPEAT : GL_CLAMP);
	if(mipmaps)
		gluBuild2DMipmaps(GL_TEXTURE_2D, type, img->w, img->h, type, GL_UNSIGNED_BYTE, img->pixels);
		//gluBuild2DMipmaps(GL_TEXTURE_2D, type, img->w, img->h, type, GL_UNSIGNED_BYTE, data);


	// Unlock the surface
	if(SDL_MUSTLOCK(img))
		SDL_UnlockSurface(img);


	// Clean up and return the texture ID
	//delete [] data;
	SDL_FreeSurface(img);

	return tex;
}


// Same as load_texture(), but loads into a specified texture index
void load_texture_into(GLuint tex, const char *ifile, const char *iimg_type, bool alpha, bool repeat, bool mipmaps) {
	// Load the 'file' to SDL_Surface
	SDL_Surface *img = NULL;
	
	char file[128];
	char img_type[128];
	
	memset(file, 0x00, sizeof(file));
	memcpy(file, ifile, strlen(ifile)+1);
	
	memset(img_type, 0x00, sizeof(img_type));
	memcpy(img_type, iimg_type, strlen(iimg_type)+1);
	
	FILE *fin = pakfile.open_file(file);
	if(!fin)
		error_msg("Unable to load texture from %s!\nSomething is wrong with the pakfile.", file);

	SDL_RWops *rw;
	rw = SDL_RWFromFP(fin, 1);
	img = IMG_LoadTyped_RW(rw,0, img_type);
	if(img == NULL)
		error_msg("Unable to load texture from %s!\n%s", file, IMG_GetError());
	SDL_FreeRW(rw);

	// Lock the surface
	if(SDL_MUSTLOCK(img))
		SDL_LockSurface(img);

	// Flip the surface pixels upside down
	Uint8 *line = new Uint8[img->w*img->format->BytesPerPixel];
	if(!line)
		error_msg("Unable to flip surface %s:\nOut of memory!\n");

	for(int f=0; f < img->h/2; f++) {
		memcpy((Uint8*)line, ((Uint8*)img->pixels + f*img->pitch), img->w*img->format->BytesPerPixel);
		memcpy(((Uint8*)img->pixels + f*img->pitch), ((Uint8*)img->pixels + (img->h - f - 1)*img->pitch), img->w*img->format->BytesPerPixel);
		memcpy(((Uint8*)img->pixels + (img->h - f - 1)*img->pitch), (Uint8*)line, img->w*img->format->BytesPerPixel);
	}

	delete [] line;


	int type = (alpha) ? GL_RGBA : GL_RGB;
	glBindTexture(GL_TEXTURE_2D, tex);

	// Upload the texture data
	glTexImage2D(GL_TEXTURE_2D, 0, type, img->w, img->h, 0, type, GL_UNSIGNED_BYTE, img->pixels);

	// Set up mip mapping
	int filter_min = (mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
	int filter_mag = GL_LINEAR;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (repeat) ? GL_REPEAT : GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (repeat) ? GL_REPEAT : GL_CLAMP);
	if(mipmaps)
		gluBuild2DMipmaps(GL_TEXTURE_2D, type, img->w, img->h, type, GL_UNSIGNED_BYTE, img->pixels);


	// Unlock the surface
	if(SDL_MUSTLOCK(img))
		SDL_UnlockSurface(img);


	// Clean up
	SDL_FreeSurface(img);
}


// Grab texture contents from the frame buffer.
// Assuming that 'tex' is a valid texture ID
// sized w*h
void grab_texture(GLuint tex, int w, int h, int mode) {

	BIND_TEXTURE(tex);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, mode, 0,0, w,h, 0);
}


// Create an empty OpenGL texture
GLuint create_empty_texture(int w, int h, int mode) {

	GLuint tex;
	GLubyte *data;
	int bpp = (mode==GL_RGB) ? 3 : 4;
	data = new GLubyte [w*h*bpp];
	int size = sizeof(GLubyte) *w *h *bpp;
	if(!data)
		error_msg("Unable to create an empty texture!");

	memset(data, 0, size);

	glGenTextures(1, &tex);		// Generate texture ID
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	delete [] data;
	return tex;
}


