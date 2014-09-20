#ifdef PLAY_AUDIO
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

int mp3Play(char* filePath);
#endif
