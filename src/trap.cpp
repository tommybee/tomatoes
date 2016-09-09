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
#include "player.h"
#include "mymath.h"
#include "particle.h"
#include "trap.h"
#include "soundmusic.h"


// Trap list
list<TRAP> traplist;

// Trap texture
GLuint trap_texture;

// Trap particle
GLuint part_trap;

// Trap time
const int trap_time = 900;


// Load the trap gfx
void load_traps() {
	trap_texture = load_png("trap.png", true, false, false);
	part_trap = load_jpg("part_trap.jpg", false, false, true);
}


// Add a trap
void add_trap(int x, int y, int owner) {
	TRAP nt;
	nt.clear();
	nt.x = x;
	nt.y = y;
	nt.owner = owner;
	nt.anim = RANDF(0,359);
	nt.alive = true;

	traplist.push_back(nt);
}


// Animate the traps
void move_traps() {
	if(traplist.size() == 0)
		return;

	list<TRAP>::iterator i;
	for(i = traplist.begin(); i != traplist.end(); ++i) {
		(*i).move();
		// Remove the dead traps
		if((*i).alive == false) {
			i = traplist.erase(i);
		}
	}

}


// Draw the traps
void draw_traps() {
	if(traplist.size() == 0)
		return;

	glDepthMask(GL_FALSE);
	BIND_TEXTURE(trap_texture);

	list<TRAP>::iterator i;
	for(i = traplist.begin(); i != traplist.end(); ++i) {
		(*i).draw();
	}

	glDepthMask(GL_TRUE);
}


// Clear the traps
void clear_traps() {
	traplist.clear();
}


// Animate the trap
void TRAP::move() {
	// Animate
	anim += 0.1f;
	if((int)anim > 3)
		anim = 0.0f;

	// Advance the time counter
	counter++;
	if(counter >= trap_time) {
		alive = false;

		// Create a explosion
		for(int f=0; f<RAND(30,40); f++) {
			VECT pos(x + 0.5f, 0.1f, y + 0.5f);
			VECT dir;
			pos += VECT(RANDF(-0.35f,0.35f),0,RANDF(-0.35f,0.35f));
			dir.x = RANDF(-0.05f, 0.05f);
			dir.y = RANDF(-0.05f, 0.05f);
			dir.y = RANDF(0.04f, 0.08f);
			float c1[4] = { 1, 0.1f, 0.1f, 1 };
			float c2[4] = { 1, 0, 0, 0 };
			add_particle(pos, dir, RAND(20,80), 0.08f, 0.4f, c1, c2, part_trap);
		}

		// Play the sound
		play_sound(SND_TRAP, false);
	}


	// Add some particles
	VECT pos(x + 0.5f, 0.1f, y + 0.5f);
	VECT dir;
	pos += VECT(RANDF(-0.35f,0.35f),0,RANDF(-0.35f,0.35f));
	dir.x = dir.z = 0.0f;
	dir.y = RANDF(0.04f, 0.07f);
	float c1[4] = { 1, 0.1f, 0.1f, 1 };
	float c2[4] = { 1, 0, 0, 0 };
	add_particle(pos, dir, RAND(10,60), 0.08f, 0.4f, c1, c2, part_trap);

}


// Draw the trap
void TRAP::draw() {
	// Translate to the position
	glPushMatrix();
	glTranslatef(x + 0.56f, 0.25f, y + 0.53f);

	// Negate the camera rotation
	glMultMatrixf(cam_neg_matrix);
//	glRotatef(45.0f, 0,1,0);
//	glRotatef(-30.0f, 1,0,0);

	// Draw the sprite
	int frame = (int)anim;
	const float siz = 0.45f;

	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.25f * frame + 0.25f, 1);	glVertex3f( siz,  siz,  siz);
		glTexCoord2f(0.25f * frame, 1);			glVertex3f(-siz,  siz,  siz);
		glTexCoord2f(0.25f * frame + 0.25f, 0);	glVertex3f( siz, -siz, -siz);
		glTexCoord2f(0.25f * frame, 0);			glVertex3f(-siz, -siz, -siz);
	glEnd();

	glPopMatrix();
}


// Clear the trap
void TRAP::clear() {
	x = y = 0;
	owner = 0;
	anim = RANDF(0,3);
	alive = false;
	counter = 0;
}
