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

#ifndef PARTICLE_H
#define PARTICLE_H

#include <list>
using namespace std;

#include "vect.h"


// Particle sprites
extern GLuint part_spark;
extern GLuint part_explo;
extern GLuint part_star;
extern GLuint part_glow;
extern GLuint part_smoke;
extern GLuint part_fire;
extern GLuint part_flower;

// "Negate camera rotation" matrix (rotates around 45 degrees around Y and around -30 degrees around X)
extern const float cam_neg_matrix[16];


// Particle class
class PARTICLE {
public:
	VECT pos;						// Position
	VECT dir;						// Speed and direction
	float rot;						// Rotation

	float size;						// Current size
	float delta_size;				// Size delta

	float color[4];					// Current color
	float delta_color[4];			// Color delta

	GLuint tex;						// Texture
	int life, maxlife;				// Current and maximum life
	bool alive;						// Is the particle alive? (i.e. active)

	// Functions
	void clear();					// Clear the particle
	void move();					// Move the particle
	void draw();					// Draw the particle
};

// Particle lists (one for regular, one for alpha blended parts)
extern list<PARTICLE> partlist;
extern list<PARTICLE> alphapartlist;


void add_particle(VECT pos, VECT dir, int maxlife, float size1, float size2, float color1[4], float color2[4], GLuint tex, bool alpha = false);
void move_particles();
void draw_particles();
void clear_particles();
void load_particles();
void kill_particles();

#endif

