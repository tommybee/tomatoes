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

#ifndef SOUNDMUSIC_H
#define SOUNDMUSIC_H

#include <fmod.h>
//#include <fmod_errors.h>

// Current music module
extern FMUSIC_MODULE *music_mod;

// Maximum number of music files
#define	MAX_MUSIC			128

// Music files array
extern char music_files[MAX_MUSIC][256];
extern int num_music_files;
extern int cur_music;


// Sound defines
#define SND_APPEAR			0
#define SND_BOMB			1
#define SND_EXPLO			2
#define SND_BONUS1			3
#define SND_BONUS2			4
#define SND_BONUS3			5
#define SND_DIE1			6
#define SND_DIE2			7
#define SND_DIE3			8
#define SND_DIE4			9
#define SND_DIE5			10
#define SND_DIE6			11
#define SND_LEVEL_TELEPORT	12
#define SND_WILDFIRE		13
#define SND_TELEPORT		14
#define SND_TRAP			15
#define SND_LIGHTNING		16
#define SND_WISP			17
#define SND_JUMP			18
#define SND_POTATOMAN		19
#define SND_POTATOMAN2		20
#define SND_TURN			21
#define SND_FLOWERPOWER		22
#define SND_KICK			23
#define SND_KILLED5			24
#define SND_MENU1			25
#define SND_MENU2			26
#define SND_FINISH			27

// Number of sounds
#define NUM_SOUNDS			28

// Sound array
extern FSOUND_SAMPLE *sounds[NUM_SOUNDS];


// Initialize the FMOD
void init_fmod();

// Play music
void play_music(char *file);

// Play a sound
void play_sound(int sound, bool random_freq);

// If the current music has finished, start playing another
void check_music_finished();


#endif
