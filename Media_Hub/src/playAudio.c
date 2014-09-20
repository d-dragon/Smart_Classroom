#ifdef PLAY_AUDIO
#include "playAudio.h"


int mp3Play(char* filePath)
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



    if (filePath==NULL)
    {
        printf(stderr, "Can not find the path of %s \n", filePath);
        return 1;
    }
    
/* intit mpg123 */

    mherr=0;
    printf("\n Open: %s\n",filePath);
   

    mpg123_init();
    mh = mpg123_new(NULL, &mherr);
    buffer_size = mpg123_outblock(mh);
    buffer = (unsigned char*) malloc(buffer_size * sizeof(unsigned char));
	
    /* open the file and get the decoding format */
    mpg123_open(mh, filePath);
    mpg123_getformat(mh, &rate, &channels, &encoding);

    printf("%d encoding %d samplerate %d channels\n", encoding,
            rate, channels);



    /* init portaudio */
    err = Pa_Initialize();
    error_check(err);

    /* we are using the default device */
    out_param.device = Pa_GetDefaultOutputDevice();
    if (out_param.device == paNoDevice)
    {
        fprintf(stderr, "Haven't found an audio device!\n");
        return -1;
    }

    /* stero or mono */
    out_param.channelCount = channels;
    out_param.sampleFormat = paInt16;
    out_param.suggestedLatency = Pa_GetDeviceInfo(out_param.device)->defaultHighOutputLatency;
    out_param.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, NULL, &out_param, rate,
         	paFramesPerBufferUnspecified, paClipOff,NULL, NULL);
    error_check(err);

    printf("\n Playing %s .....",filePath);

    err = Pa_StartStream(stream);
    error_check(err);

 
        

    /* decode and play */

    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK)
    {
        Pa_WriteStream(stream, buffer,done/4);
       
    }

    printf("\n Finish playing !!!!!!\n");

    
    error_check(err);

   
   err = Pa_StopStream(stream);
   error_check(err);

    err = Pa_CloseStream(stream);
    error_check(err);

    free(buffer);
    
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();

    Pa_Terminate();

    return 0;
}
#endif
