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
#include "SDL.h"
#include "game.h"
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <fmod.h>
#include <fmod_errors.h>
#include "soundmusic.h"
#include "mymath.h"
#include "init.h"
#include "mpak.h"
#include "comments.h"
#include "timer.h"

// Current music module
FMUSIC_MODULE *music_mod;

// Music files array
char music_files[MAX_MUSIC][256];
int num_music_files;
int cur_music;

// Sound array
FSOUND_SAMPLE *sounds[NUM_SOUNDS];


// Shuffle the playlist
void shuffle_playlist() {
	if(!num_music_files)
		return;

	cur_music = 0;

	// Shuffle the list
	for(int i = num_music_files - 1; i > 0; i--) {
		int pos = RAND(0, i - 1);
		char swap[256];
		strcpy(swap, music_files[i]);
		strcpy(music_files[i], music_files[pos]);
		strcpy(music_files[pos], swap);
	}
}


// Search for the music files
void search_music() {
	DIR *dp;
	dirent *ep;

	// Search files from the music directory
	dp = opendir(MUSIC_DIR);
	if(!dp || !config.sound || !config.music_vol) {
		// No files found, or the sound is turned off
		num_music_files = 0;
		return;
	}

	// Clear music file list
	num_music_files = 0;
	cur_music = 0;
	for(int f=0; f<MAX_MUSIC; f++)
		strcpy(music_files[f], "");

	// Start searching
	while((ep = readdir(dp)) ) {
		if(num_music_files >= MAX_MUSIC-1) {
			printf("Warning: Too many music files in '%s' directory!\n", MUSIC_DIR);
			break;
		}

		// Check the extension
		char ext[3];
		char name[256] = "";
		strcpy(name, ep->d_name);
		int len = strlen(name);
		if(len > 3) {
			ext[0] = name[len-3];
			ext[1] = name[len-2];
			ext[2] = name[len-1];
			if(toupper(ext[0]) == 'M' && toupper(ext[1]) == 'O' && toupper(ext[2]) == 'D') {
				// Found MOD
				strcpy(music_files[num_music_files], name);
				num_music_files++;
			}

			if(toupper(ext[0]) == 'S' && toupper(ext[1]) == '3' && toupper(ext[2]) == 'M') {
				// Found S3M
				strcpy(music_files[num_music_files], name);
				num_music_files++;
			}

			if(toupper(ext[1]) == 'X' && toupper(ext[2]) == 'M') {
				// Found XM
				strcpy(music_files[num_music_files], name);
				num_music_files++;
			}

			if(toupper(ext[1]) == 'I' && toupper(ext[2]) == 'T') {
				// Found IT
				strcpy(music_files[num_music_files], name);
				num_music_files++;
			}
		}
	}

	closedir(dp);
	shuffle_playlist();
}


// If the current music has finished, start playing another
void check_music_finished() {
	static bool f1_key_down = false;

	if(!config.sound || !num_music_files || !config.music_vol)
		return;

	if(FMUSIC_IsFinished(music_mod) || key[SDLK_F1]) {
		if(f1_key_down == false) {
			// Play the current song from the playlist
			play_music(music_files[cur_music]);

			cur_music++;
			if(cur_music > num_music_files-1)
				shuffle_playlist();

			f1_key_down = true;
		}
	}

	if(!key[SDLK_F1])
		f1_key_down = false;
}


// This helper function loads a sound and stores it to the sound array
static int cur_sound = 0;
void load_sound(const char *ifile) {
	
	char file[256];
	
	memset(file, 0x00, sizeof(file));
	
	memcpy(file, ifile, strlen(ifile)+1);
	
	if(cur_sound > NUM_SOUNDS-1)
		error_msg("load_sounds():\nTrying to load too many sounds!\nNUM_SOUNDS is defined as %d.\n", NUM_SOUNDS);

	// First try to load from the override directory
	char soundfile[128] = "";
	sprintf(soundfile, "%s%s", pakfile.override_dir, file);
	FILE *check = fopen(soundfile, "rb");
	if(check) {
		// The file exists, load it
		fclose(check);
		sounds[cur_sound] = FSOUND_Sample_Load(FSOUND_FREE, soundfile, FSOUND_2D, 0, 0);
	}
	else {
		// The file doesn't exist in the override directory, try to load it
		// from the pakfile.

		// Find the correct offset and the sample length
		UINT32 offset = 0, len = 0;
		int idx = pakfile.find_file(file);
		if(idx == -1)
			error_msg("load_sound():\nUnable to load sound %s from the pakfile!\nThe sound doesn't exist!", file);
		offset = pakfile.offsets[idx];
		len = pakfile.sizes[idx];

		// Load the sample
		sounds[cur_sound] = FSOUND_Sample_Load(FSOUND_FREE, pakfile.mpk_file, FSOUND_2D, offset, len);
	}

	// Check for errors
	if(!sounds[cur_sound])
		error_msg("load_sound():\nUnable to load a sound from %s!\n", file);

	cur_sound++;
}


// Initialize the FMOD
void init_fmod() {
	// Check the FMOD version
	if(FSOUND_GetVersion() < FMOD_VERSION)
		error_msg("You are using a wrong version of FMOD.DLL (v%.02f)\nYou should have FMOD.DLL version v%.02f\n", FSOUND_GetVersion(), FMOD_VERSION);

	// Check if the audio is disabled
	if(!config.sound)
		FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);		// Disable the audio

	// Initialize the FMOD
	if(!FSOUND_Init(config.sound_freq, 128, FSOUND_INIT_GLOBALFOCUS))
		error_msg("Unable to initialize FMOD: FSOUND_Init() failed!\nReason of failure: %s\n", FMOD_ErrorString(FSOUND_GetError()));

	// Load the sounds
	if(config.sound) {
		load_sound("snd_appear.wav");
		load_sound("snd_bomb.wav");
		load_sound("snd_explo.wav");
		load_sound("snd_bonus1.wav");
		load_sound("snd_bonus2.wav");
		load_sound("snd_bonus3.wav");
		load_sound("snd_die1.wav");
		load_sound("snd_die2.wav");
		load_sound("snd_die3.wav");
		load_sound("snd_die4.wav");
		load_sound("snd_die5.wav");
		load_sound("snd_die6.wav");
		load_sound("snd_levelteleport.wav");
		load_sound("snd_wildfire.wav");
		load_sound("snd_teleport.wav");
		load_sound("snd_trap.wav");
		load_sound("snd_lightning.wav");
		load_sound("snd_wisp.wav");
		load_sound("snd_jump.wav");
		load_sound("snd_potatoman.wav");
		load_sound("snd_potatoman2.wav");
		load_sound("snd_turn.wav");
		load_sound("snd_flowerpower.wav");
		load_sound("snd_kick.wav");
		load_sound("snd_killed5.wav");
		load_sound("snd_menu1.wav");
		load_sound("snd_menu2.wav");
		load_sound("snd_finish.wav");
	}

	// Search for music files
	search_music();

	// Set the volume
	FSOUND_SetSFXMasterVolume(config.sound_vol);

	// Start playing the music
	if(num_music_files) {
		play_music(music_files[0]);
		cur_music++;
		if(cur_music > num_music_files-1)
			cur_music = 0;
	}
}


// Play music
void play_music(char *file) {
	if(!config.sound || !config.music_vol)
		return;

	game_paused = true;

	char str[256] = "";

	sprintf(str, "%s%s", MUSIC_DIR, file);
	FMUSIC_StopSong(music_mod);
	FMUSIC_FreeSong(music_mod);
	music_mod = FMUSIC_LoadSong(str);
	if(!music_mod)
		error_msg("play_music():\nUnable to play music from '%s'!\nFMOD error: %s\n", file, FMOD_ErrorString(FSOUND_GetError()));

	// Play
	FMUSIC_PlaySong(music_mod);
	
	// Add the comment
	add_comment(COL_DEFAULT, "Playing \"%s\"...", FMUSIC_GetName(music_mod));

	// Set the volume
	FMUSIC_SetMasterVolume(music_mod, config.music_vol);

	game_paused = false;
}


// Play a sound
void play_sound(int sound, bool random_freq) {
	if(!config.sound)
		return;

	// Play the sound
	int channel = FSOUND_PlaySound(FSOUND_FREE, sounds[sound]);

	// Random the frequency
	if(random_freq) {
		int freq;
		int freq_change[2];
		freq = FSOUND_GetFrequency(channel);

		freq_change[0] = (int)((float)freq * 0.3f);
		freq_change[1] = (int)((float)freq * 0.1f);

		// Upload the frequency to the sample
		FSOUND_SetFrequency(channel, freq+RAND(-freq_change[0], freq_change[1]));
	}
}
