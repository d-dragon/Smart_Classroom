
#ifndef __PLAY_AUDIO_H__
#define __PLAY_AUDIO_H__

#ifdef PLAY_AUDIO
#include <portaudio.h>
#include <mpg123.h>
#endif

#include <stdio.h>
#include <pthread.h>

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

typedef enum g_audio_status{
	AUDIO_STOP,
	AUDIO_PLAY,
	AUDIO_PAUSE,
	AUDIO_ERROR
}g_audio_status;

#ifdef PLAY_AUDIO
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
#endif
pthread_t g_play_audio_thd;
pthread_mutex_t g_audio_status_mutex;
g_audio_status g_audio_flag;

#ifdef PLAY_AUDIO
int play(mp3Player* player);// play and stop an mp3Player play(player);see ex in main function
int stop(mp3Player* player);//
#endif

int playAudio(char *message);
int initAudioPlayer(char *filename);
void *playAudioThread(void *);
int stopAudio();
int pauseAudio();

#endif

