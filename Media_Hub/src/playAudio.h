#ifndef __PLAY_AUDIO_H__
#define __PLAY_AUDIO_H__

#include <portaudio.h>
#include <mpg123.h>
#include <stdio.h>

#define error_check(err) \
    do {\
        if (err) { \
            fprintf(stderr, "line %d ", __LINE__); \
            fprintf(stderr, "error number: %d\n", err); \
            fprintf(stderr, "\n\t%s\n\n", Pa_GetErrorText(err)); \
            return err; \
        } \
    } while (0)

typedef int bool;
#define true 1
#define false 0

typedef struct player
{
	PaStreamParameters out_param;
    PaStream * stream;
    PaError err;
   
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int mherr;
   
    int channels, encoding;
    long rate;
	char* fileName;
	
	bool play;
	
	
	
	

	

}mp3Player;

int play(mp3Player* player);// play and stop an mp3Player play(player);see ex in main function
int stop(mp3Player* player);//
#endif
