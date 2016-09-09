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

#include <stdio.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "SDL_image.h"
#include "texture.h"
#include "init.h"
#include "mymath.h"
#include "tilemap.h"
#include "particle.h"

// Particle display list
GLuint part_dlist;


// Particle sprites
GLuint part_spark;
GLuint part_explo;
GLuint part_star;
GLuint part_glow;
GLuint part_smoke;
GLuint part_fire;
GLuint part_flower;


// Gravity
const float gravity = -0.002f;

// "Negate camera rotation" matrix (rotates 45 degrees around Y axis and -30 degrees around X axis)
const float cam_neg_matrix[16] = { 0.707107f, 0, -0.707107f, 0, -0.353553f, 0.866025f, -0.353553f, 0, 0.612372f, 0.5f, 0.612372f, 0, 0, 0, 0, 1 };


// Particle lists (one for regular, one for alpha blended parts)
list<PARTICLE> partlist;
list<PARTICLE> alphapartlist;



// Load particles
void load_particles() {
	part_spark = load_jpg("spark.jpg", false, false, true);
	part_explo = load_jpg("explo.jpg", false, false, true);
	part_star = load_jpg("star.jpg", false, false, true);
	part_glow = load_jpg("glow.jpg", false, false, true);
	part_smoke = load_png("smoke.png", true, false, true);
	part_fire = load_jpg("fire.jpg", false, false, true);
	part_flower = load_png("flower.png", true, false, true);


	// Build the display list
	part_dlist = glGenLists(1);
	glNewList(part_dlist, GL_COMPILE);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 1,  1,  1);
		glTexCoord2f(0,1); glVertex3f(-1,  1,  1);
		glTexCoord2f(1,0); glVertex3f( 1, -1, -1);
		glTexCoord2f(0,0); glVertex3f(-1, -1, -1);
	glEnd();
	glEndList();
}


// Free the particle list
void kill_particles() {
	glDeleteLists(part_dlist, 1);
}


// Add particle
void add_particle(VECT pos, VECT dir, int maxlife, float size1, float size2, float color1[4], float color2[4], GLuint tex, bool alpha) {
	PARTICLE p;
	p.clear();
	p.pos = pos;
	p.dir = dir;
	p.size = size1;
	p.delta_size = (size2 - size1) / (float)maxlife;
	p.life = 0;
	p.maxlife = maxlife;
	p.alive = true;
	p.tex = tex;

	for(int f=0; f<4; f++) {
		p.color[f] = color1[f];
		p.delta_color[f] = (color2[f] - color1[f]) / (float)maxlife;
	}

	// Regular particle or alpha blended particle?
	if(alpha)
		alphapartlist.push_back(p);
	else
		partlist.push_back(p);
}


// Move the particles
void move_particles() {
	// Move the regular particles
	if(partlist.size() != 0) {
		list<PARTICLE>::iterator i;
		for(i = partlist.begin(); i != partlist.end(); ++i) {
			(*i).move();
			// Remove the dead particles
			if((*i).alive == false) {
				i = partlist.erase(i);
			}
		}
	}

	// Move the alpha blended particles
	if(alphapartlist.size() != 0) {
		list<PARTICLE>::iterator i;
		for(i = alphapartlist.begin(); i != alphapartlist.end(); ++i) {
			(*i).move();
			// Remove the dead particles
			if((*i).alive == false) {
				i = alphapartlist.erase(i);
			}
		}
	}

}


// Draw the particles
void draw_particles() {
	// Draw the regular particles
	if(partlist.size() != 0) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);

		list<PARTICLE>::iterator i;
		for(i = partlist.begin(); i != partlist.end(); ++i)
			(*i).draw();

		glDepthMask(GL_TRUE);
	}

	// Draw the alpha blended particles
	if(alphapartlist.size() != 0) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);

		list<PARTICLE>::iterator i;
		for(i = alphapartlist.begin(); i != alphapartlist.end(); ++i)
			(*i).draw();

		glDepthMask(GL_TRUE);
	}
}


// Clear the particles
void clear_particles() {
	partlist.clear();
	alphapartlist.clear();
}


// Draw the particle
void PARTICLE::draw() {
	// Translate to the position
	glPushMatrix();
	glTranslatef(pos.x, pos.y, pos.z);


	// Negate the camera rotation
	glMultMatrixf(cam_neg_matrix);
//	glRotatef(45.0f, 0,1,0);
//	glRotatef(-30.0f, 1,0,0);

	glRotatef(rot, 0,0,1);

	// Draw the sprite
	BIND_TEXTURE(tex);
	glColor4fv(color);
	glScalef(size, size, size);
	glCallList(part_dlist);
/*	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( size,  size,  size);
		glTexCoord2f(0,1); glVertex3f(-size,  size,  size);
		glTexCoord2f(1,0); glVertex3f( size, -size, -size);
		glTexCoord2f(0,0); glVertex3f(-size, -size, -size);
	glEnd();
*/
	glPopMatrix();
}


// Move the particle
void PARTICLE::move() {
	// Advance the life
	life++;
	if(life >= maxlife) {
		alive = false;
		return;
	}

	// Model some gravity and move
	dir.y += gravity;
	pos += dir;

	// Bounce from the floor
	if(pos.y < 0.0f) {
		pos.y = 0.0f;
		dir.y = -dir.y;
	}


	// Modify the size
	size += delta_size;

	// Modify the color
	color[0] += delta_color[0];
	color[1] += delta_color[1];
	color[2] += delta_color[2];
	color[3] += delta_color[3];

	// Rotate
	//rot = add_angle(rot, 2.0f);
	rot += 2.0f;
	//if(rot > 360.0f)
	//	rot -= 360.0f;

}


// Clear the particle
void PARTICLE::clear() {
	pos = 0.0f;
	dir = 0.0f;
	rot = RANDF(0,359);
	size = 1.0f;
	delta_size = 0.0f;

	color[0] = color[1] = color[2] = color[3] = 1.0f;
	delta_color[0] = delta_color[1] = delta_color[2] = delta_color[3] = 0.0f;

	tex = 0;
	life = maxlife = 0;
	alive = false;
}


