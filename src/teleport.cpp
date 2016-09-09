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
#include "player.h"
#include "particle.h"
#include "tilemap.h"
#include "mymath.h"
#include "teleport.h"
#include "soundmusic.h"
#include "game.h"


// Teleport textures
GLuint teleport_tex;
GLuint part_teleport;

// Teleport animation stuff
float teleport_frame[2] = { 0, 2 };


// Animate the teleports
void animate_teleports() {
	// Check if the players enter these teleports
	// For player #1
	if(p1.x == teleport_x[0] && p1.y == teleport_y[0] && !p1.in_teleport) {
		// Jump ahead from teleport #1
		p1.jump(teleport_x[1], teleport_y[1], 5.0f, 0.01f);
		p1.in_teleport = 1;
		play_sound(SND_LEVEL_TELEPORT, false);
	}
	else if(p1.x == teleport_x[1] && p1.y == teleport_y[1] && !p1.in_teleport) {
		// Jump ahead from teleport #2
		p1.jump(teleport_x[0], teleport_y[0], 5.0f, 0.01f);
		p1.in_teleport = 2;
		play_sound(SND_LEVEL_TELEPORT, false);
	}

	// For player #2
	if(two_players && p2.x == teleport_x[0] && p2.y == teleport_y[0] && !p2.in_teleport) {
		// Jump ahead from teleport #1
		p2.jump(teleport_x[1], teleport_y[1], 5.0f, 0.01f);
		p2.in_teleport = 1;
		play_sound(SND_LEVEL_TELEPORT, false);
	}
	else if(two_players && p2.x == teleport_x[1] && p2.y == teleport_y[1] && !p2.in_teleport) {
		// Jump ahead from teleport #2
		p2.jump(teleport_x[0], teleport_y[0], 5.0f, 0.01f);
		p2.in_teleport = 2;
		play_sound(SND_LEVEL_TELEPORT, false);
	}


	// Animate
	teleport_frame[0] += 0.1f;
	if((int)teleport_frame[0] > 3)
		teleport_frame[0] = 0.0f;

	teleport_frame[1] += 0.1f;
	if((int)teleport_frame[1] > 3)
		teleport_frame[1] = 0.0f;


	// Add some particle effects
	if(teleport_x[0] != -1) {
		VECT pos(teleport_x[0] + 0.56f, 0.5f, teleport_y[0] + 0.53f);
		VECT dir;
		pos += VECT(RANDF(-0.35f,0.35f),0,RANDF(-0.35f,0.35f));
		dir.x = dir.z = 0.0f;
		dir.y = RANDF(0.04f, 0.07f);
		float c1[4] = { 0.3, 0.7f, 1, 1 };
		float c2[4] = { 0, 0, 1, 0 };
		add_particle(pos, dir, RAND(10,60), 0.08f, 0.4f, c1, c2, part_teleport);
	}

	if(teleport_x[1] != -1) {
		VECT pos(teleport_x[1] + 0.56f, 0.5f, teleport_y[1] + 0.53f);
		VECT dir;
		pos += VECT(RANDF(-0.35f,0.35f),0,RANDF(-0.35f,0.35f));
		dir.x = dir.z = 0.0f;
		dir.y = RANDF(0.04f, 0.07f);
		float c1[4] = { 0.3, 0.7f, 1, 1 };
		float c2[4] = { 0, 0, 1, 0 };
		add_particle(pos, dir, RAND(10,60), 0.08f, 0.4f, c1, c2, part_teleport);
	}
}


// Load the teleports
void load_teleports() {
	teleport_tex = load_png("teleport1.png", true, false, false);
	part_teleport = load_jpg("teleport2.jpg", false, false, true);
}


// Draw the teleports
void draw_teleports() {

	glColor3f(1,1,1);
	glDepthMask(GL_FALSE);
	BIND_TEXTURE(teleport_tex);

	// Draw the teleport #1
	if(teleport_x[0] != -1) {

		// Translate to the position
		glPushMatrix();
		glTranslatef(teleport_x[0] + 0.56f, 0.25f, teleport_y[0] + 0.53f);

		// Negate the camera rotation
		glMultMatrixf(cam_neg_matrix);
//		glRotatef(45.0f, 0,1,0);
//		glRotatef(-30.0f, 1,0,0);

		// Draw the sprite
		int frame = (int)teleport_frame[0];
		const float siz = 0.5f;

		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.25f * frame + 0.25f, 1);	glVertex3f( siz,  siz,  siz);
			glTexCoord2f(0.25f * frame, 1);			glVertex3f(-siz,  siz,  siz);
			glTexCoord2f(0.25f * frame + 0.25f, 0);	glVertex3f( siz, -siz, -siz);
			glTexCoord2f(0.25f * frame, 0);			glVertex3f(-siz, -siz, -siz);
		glEnd();

		glPopMatrix();
	}

	// Draw the teleport #2
	if(teleport_x[1] != -1) {

		// Translate to the position
		glPushMatrix();
		glTranslatef(teleport_x[1] + 0.56f, 0.25f, teleport_y[1] + 0.53f);

		// Negate the camera rotation
		glMultMatrixf(cam_neg_matrix);
//		glRotatef(45.0f, 0,1,0);
//		glRotatef(-30.0f, 1,0,0);

		// Draw the sprite
		int frame = (int)teleport_frame[1];
		const float siz = 0.5f;

		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.25f * frame + 0.25f, 1);	glVertex3f( siz,  siz,  siz);
			glTexCoord2f(0.25f * frame, 1);			glVertex3f(-siz,  siz,  siz);
			glTexCoord2f(0.25f * frame + 0.25f, 0);	glVertex3f( siz, -siz, -siz);
			glTexCoord2f(0.25f * frame, 0);			glVertex3f(-siz, -siz, -siz);
		glEnd();

		glPopMatrix();
	}

	glDepthMask(GL_TRUE);

}

