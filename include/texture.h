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

#ifndef TEXTURE_H
#define TEXTURE_H

// Macro for texture binding (yes, I'm *that* lazy!)
#define BIND_TEXTURE(x)		glBindTexture(GL_TEXTURE_2D, x)


// Load a PNG
GLuint load_png(const char *file, bool alpha = false, bool repeat = false, bool mipmaps = true);

// Load a JPG
GLuint load_jpg(const char *file, bool alpha = false, bool repeat = false, bool mipmaps = true);


// Load a image file using SDL_Image and
// convert it to OpenGL texture.
// If alpha == true -> RGBA alpha texture
// If repeat == true -> texture can be tiled
// If mipmaps == true -> Mipmaps will be generated
// Return texture ID
GLuint load_texture(const char *file, const char *type, bool alpha = false, bool repeat = false, bool mipmaps = true);

// Same as load_texture(), but loads into the specified texture index
void load_texture_into(GLuint tex, const char *file, const char *img_type, bool alpha, bool repeat, bool mipmaps);

// Grab texture contents from the frame buffer.
// Assuming that 'tex' is a valid texture ID
// sized w*h
void grab_texture(GLuint tex, int w, int h, int mode);

// Create an empty OpenGL texture
GLuint create_empty_texture(int w, int h, int mode);

#endif


