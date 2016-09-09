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
#include "mymath.h"
#include "tilemap.h"
#include "particle.h"
#include "texture.h"
#include "effects.h"


// Create the teleport effect
void create_teleport_effect(int x, int y) {

	// Add the big glow particle
	VECT pos(x + 0.5f, 0.5f, y + 0.5f);
	VECT dir = 0.0f;
	float c1[4] = { 1.0f, 1.0f, 0.3f, 1.0f };
	float c2[4] = { 1, 1, 0, 0 };
//	float c1[4] = { 0.3f, 0.8f, 1.0f, 1.0f };
//	float c2[4] = { 0, 0, 1, 0 };
	add_particle(pos, dir, 60, 2.0f, 1.0f, c1, c2, part_glow);

	// Create those "fire lines"
	for(int f=0; f < RAND(5,6); f++) {
	    // Choose the direction
		VECT dir;
		dir.x = RANDF(-0.5f, 0.5f);
		dir.y = RANDF(0.05f, 0.5f);
		dir.z = RANDF(-0.5f, 0.5f);
		normalize(dir);

		// Create the particles
		int max = RAND(25, 50);
		for(int i=0; i < max; i++) {
			VECT p(x + 0.5f, 0.0f, y + 0.5f);
			p += (dir * (float)i * 0.1f);
			VECT d = 0.0f;
			float s = (float)i / (float)max;
			add_particle(p, d, RAND(40,80), 0.5f * (1.0f-s), 0.00f, c1, c2, part_glow);
		}

	}

}


// Create some particle fire
void create_fire(VECT pos) {
	VECT ppos = pos;

	// Create some particle fire
	for(int i=0; i<RAND(3,7); i++) {
		ppos = pos;
		ppos.x += RANDF(-0.4f, 0.4f);
		ppos.y += RANDF(-0.3f, 0.3f);
		ppos.z += RANDF(-0.4f, 0.4f);
		VECT dir(RANDF(-0.01f,0.01f), RANDF(0.05f,0.15f), RANDF(-0.01f,0.01f));
		float c1[4] = { 1, 0.5f, 0.1f, 1 };
		float c2[4] = { 1, 0.1f, 0.1f, 0.1f };
		add_particle(ppos, dir, RAND(20,35), 0.3f, 0.1f, c1, c2, part_fire);
	}

	// Create smoke
	if(RAND(0,100) > 50) {
		pos.y += 0.2f;
		pos.x += RANDF(-0.3f, 0.3f);
		pos.y += RANDF(0.1f, 0.5f);
		pos.z += RANDF(-0.3f, 0.3f);
		VECT dir(RANDF(-0.01f,0.01f), RANDF(0.05f,0.1f), RANDF(-0.01f,0.01f));
		float c1[4] = { 0.5f, 0.1f, 0.1f, 1 };
		float c2[4] = { 0.1f, 0.1f, 0.1f, 0 };
		add_particle(pos, dir, RAND(20,35), 0.2f, 0.4f, c1, c2, part_smoke, true);
	}
}


// Helper function for the lightning effect.
// Creates an recursive lightning bolt
void light_bolt(VECT pos1, VECT pos2, int points, float noise, float thickness, int level, float color[4]) {
	if(level > 6)
		return;			// Don't recurse too deeply

	// Create the points array
	VECT *point = new VECT[points];
	if(!point)
		return;

	// Define the end points
	point[0] = pos1;
	point[points-1] = pos2;

	// Compute the direction from pos1 to pos2
	VECT dir = pos2 - pos1;
	float dist = vector_length(dir);
	float step = dist / points;
	normalize(dir);

	// Make a straight line between pos1 and pos2, and randomize it a bit
	for(int f=1; f < points-1; f++) {
		point[f] = point[f-1] + step * dir;

		point[f].x += RANDF(-noise, noise);
		point[f].y += RANDF(-noise, noise);
		point[f].z += RANDF(-noise, noise);

		// Create some recursive bolts
		if(RAND(0,100) > 45) {
			// Choose a destination which isn't too close to the original point
			VECT dest;
			dest.x = RANDF(-noise,noise);
			dest.y = RANDF(-noise,noise);
			dest.z = RANDF(-noise,noise);
			bool ok = false;
			while(!ok) {
				float bdist = vector_length(VECT(dest - point[f]));
				float rad = noise * 0.5f;
				if(bdist > rad) {
					// Also try to steer the bolts towards the ground
					if((point[f].y + dest.y) < point[f].y)
						ok = true;
					else
						ok = false;
				}
				if(!ok) {
					dest.x = RANDF(-noise,noise);
					dest.y = RANDF(-noise,noise);
					dest.z = RANDF(-noise,noise);
				}
			}

			dest *= RANDF(3.0f, 7.0f);
			light_bolt(point[f], point[f] + dest, int(points * 0.65f), noise * 0.5f, thickness * 0.5f, level + 1, color);
		}
	}

	glColor4fv(color);
	glLineWidth(thickness);
	glBegin(GL_LINE_STRIP);
	for(int f=0; f<points; f++) {
	//for(int f=0; f<points-1; f++) {
		glVertex3f(point[f].x, point[f].y, point[f].z);
		//glVertex3f(point[f+1].x, point[f+1].y, point[f+1].z);
	}
	glEnd();

	// Draw some glows
	if(level == 0) {
		BIND_TEXTURE(part_glow);
		glColor4f(.4f, .8f, 1, RANDF(.15f,.25f));
		for(int f=0; f<points; f++) {
			glPushMatrix();
			glTranslatef(point[f].x, point[f].y, point[f].z);
			// Negate the camera rotation
			glRotatef(45.0f, 0,1,0);
			glRotatef(-30.0f, 1,0,0);
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(1,1); glVertex3f( 1,  1,  1);
				glTexCoord2f(0,1); glVertex3f(-1,  1,  1);
				glTexCoord2f(1,0); glVertex3f( 1, -1, -1);
				glTexCoord2f(0,0); glVertex3f(-1, -1, -1);
			glEnd();
			glPopMatrix();
		}
		BIND_TEXTURE(0);
	}

	delete [] point;
}


// The lightning effect
void draw_lightning(VECT pos1, VECT pos2, float noise1, float noise2) {

	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	BIND_TEXTURE(0);

	// Draw two bolts
	float color[4] = { .5f, .8f, 1, 1 };
	light_bolt(pos1, pos2, 15, noise1, 5, 0, color);		// noise: 0.35f

	float color2[4] = { .2f, .6f, 1, 1 };
	light_bolt(pos1, pos2, 10, noise2, 3, 1, color2);		// noise: 0.20f

	// Draw the end point glows
	BIND_TEXTURE(part_glow);
	glColor4f(.4f, .8f, 1, RANDF(.5f,1));
	glPushMatrix();
	glTranslatef(pos1.x, pos1.y, pos1.z);
	// Negate the camera rotation
	glRotatef(45.0f, 0,1,0);
	glRotatef(-30.0f, 1,0,0);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 1.2f,  1.2f,  1.2f);
		glTexCoord2f(0,1); glVertex3f(-1.2f,  1.2f,  1.2f);
		glTexCoord2f(1,0); glVertex3f( 1.2f, -1.2f, -1.2f);
		glTexCoord2f(0,0); glVertex3f(-1.2f, -1.2f, -1.2f);
	glEnd();
	glPopMatrix();

/*	glPushMatrix();
	glTranslatef(pos2.x, pos2.y, pos2.z);
	// Negate the camera rotation
	glRotatef(45.0f, 0,1,0);
	glRotatef(-30.0f, 1,0,0);
	glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(1,1); glVertex3f( 1.2f,  1.2f,  1.2f);
		glTexCoord2f(0,1); glVertex3f(-1.2f,  1.2f,  1.2f);
		glTexCoord2f(1,0); glVertex3f( 1.2f, -1.2f, -1.2f);
		glTexCoord2f(0,0); glVertex3f(-1.2f, -1.2f, -1.2f);
	glEnd();
	glPopMatrix();
*/
	glEnable(GL_DEPTH_TEST);
}


// Create an explosion to a tile
void create_explosion(int x, int y, int type) {
	int p;
	if(type == EXP_BOMB_NORMAL) {
		for(p = 0; p < RAND(20,40); p++) {
			// Add the explosion flames
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.01f, 0.01f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.01f, 0.01f);
			float c1[4] = { 1, 0.3f, 0.2f, 1 };
			float c2[4] = { 1, 0, 0, 0 };
			add_particle(pos, dir, RAND(10,60), 0.2f, 0.8f, c1, c2, part_explo);
		}
		for(p = 0; p < RAND(10,30); p++) {
			// Add the sparks
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.05f, 0.05f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.05f, 0.05f);
			float c1[4] = { 1, 0.7f, 0.3f, 1 };
			float c2[4] = { 0, 0, 1, 0 };
			add_particle(pos, dir, RAND(20,70), 0.05f, 0.01f, c1, c2, part_spark);
		}
	}

	if(type == EXP_BOMB_FLOWER) {
		for(p = 0; p < RAND(10,30); p++) {
			// Add the explosion flames
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.01f, 0.01f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.01f, 0.01f);
			float c1[4] = { 0.5f, 0, 1, 1 };
			float c2[4] = { 0.2f, 1, 0.2f, 0 };
			add_particle(pos, dir, RAND(10,60), 0.2f, 0.8f, c1, c2, part_explo);
		}
		for(p = 0; p < RAND(1,5); p++) {
			// Add the flowers
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.05f, 0.05f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.05f, 0.05f);
			float c1[4] = { 1, 1, 1, 1 };
			float c2[4] = { 1, 1, 1, 0 };
			add_particle(pos, dir, RAND(50,100), 0.5f, 0.1f, c1, c2, part_flower, true);
		}
	}

	else if(type == EXP_BOMB_CENTER) {
		for(p = 0; p < RAND(30,70); p++) {
			// Add the explosion flames
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.01f, 0.01f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.01f, 0.01f);
			float c1[4] = { 1, 0.7f, 0.3f, 1 };
			float c2[4] = { 1, 0, 0, 0 };
			add_particle(pos, dir, RAND(10,60), 0.2f, 0.8f, c1, c2, part_explo);
		}
		for(p = 0; p < RAND(10,40); p++) {
			// Add the sparks
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.05f, 0.05f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.05f, 0.05f);
			float c1[4] = { 1, 0.7f, 0.3f, 1 };
			float c2[4] = { 0, 0, 1, 0 };
			add_particle(pos, dir, RAND(20,70), 0.05f, 0.01f, c1, c2, part_spark);
		}
	}

	else if(type == EXP_BOMB_CENTER_FLOWER) {
		for(p = 0; p < RAND(20,30); p++) {
			// Add the explosion flames
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.01f, 0.01f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.01f, 0.01f);
			float c1[4] = { 0.3f, 1, 0.3f, 1 };
			float c2[4] = { .5f, 0, 1, 0 };
			add_particle(pos, dir, RAND(10,60), 0.2f, 0.8f, c1, c2, part_explo);
		}
		for(p = 0; p < RAND(1,5); p++) {
			// Add the flowers
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.05f, 0.05f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.05f, 0.05f);
			float c1[4] = { 1, 1, 1, 1 };
			float c2[4] = { 1, 1, 1, 0 };
			add_particle(pos, dir, RAND(50,100), 0.5f, 0.1f, c1, c2, part_flower, true);
		}
	}

	else if(type == EXP_NAPALM) {
		for(p = 0; p < RAND(20,50); p++) {
			// Add the explosion flames
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.01f, 0.01f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.01f, 0.01f);
			float c1[4] = { 1, 0.3f, 0.2f, 1 };
			float c2[4] = { 1, 0, 0, 0 };
			add_particle(pos, dir, RAND(10,60), 0.2f, 0.8f, c1, c2, part_explo);
		}
		for(p = 0; p < RAND(10,30); p++) {
			// Add the sparks
			VECT pos(x + 0.5f, 0.0f, y + 0.5f);
			VECT dir;
			dir.x = RANDF(-0.05f, 0.05f);
			dir.y = RANDF(0.0f, 0.1f);
			dir.z = RANDF(-0.05f, 0.05f);
			float c1[4] = { 1, 0.7f, 0.3f, 1 };
			float c2[4] = { 0, 0, 1, 0 };
			add_particle(pos, dir, RAND(20,70), 0.05f, 0.01f, c1, c2, part_spark);
		}
	}
}
