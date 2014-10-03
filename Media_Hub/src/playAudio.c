
#include "playAudio.h"


int play(mp3Player* player)
{
		player->play = true;
		int i = 0;
		player->mherr=0;
		printf("\n Open:%s\n",player->fileName);
	   

		mpg123_init();
		player->mh = mpg123_new(NULL, &(player->mherr));
		player->buffer_size = mpg123_outblock(player->mh);
		player->buffer = (unsigned char*) malloc((player->buffer_size) * sizeof(unsigned char));
		
		/* open the file and get the decoding format */
		mpg123_open(player->mh, player->fileName);
		mpg123_getformat(player->mh, &(player->rate), &(player->channels), &(player->encoding));

		printf("%d encoding %d samplerate %d channels\n", player->encoding,
				player->rate, player->channels);



		/* init portaudio */
		player->err = Pa_Initialize();
		error_check(player->err);

		/* we are using the default device */
		player->out_param.device = Pa_GetDefaultOutputDevice();
		if (player->out_param.device == paNoDevice)
		{
			fprintf(stderr, "Haven't found an audio device!\n");
			return -1;
		}

		/* stero or mono */
		player->out_param.channelCount = player->channels;
		player->out_param.sampleFormat = paInt16;
		player->out_param.suggestedLatency = Pa_GetDeviceInfo(player->out_param.device)->defaultHighOutputLatency;
		player->out_param.hostApiSpecificStreamInfo = NULL;

		player->err = Pa_OpenStream(&(player->stream), NULL, &(player->out_param), player->rate,
				paFramesPerBufferUnspecified, paClipOff,NULL, NULL);
		error_check(player->err);

		printf("\n Playing %s .....",player->fileName);

		player->err = Pa_StartStream(player->stream);
		error_check(player->err);

	 
			

		//err = Pa_SetStreamFinishedCallback(stream, &end_cb);
		//error_check(err);

		/* decode and play */

		printf("\n Playing %s .....",player->fileName);
		while (player->play)
		{
			if(mpg123_read(player->mh, player->buffer, player->buffer_size, &player->done) == MPG123_OK)
			{
				Pa_WriteStream(player->stream, player->buffer,(player->done)/4);
				i++;
				if(i == 50)
				{
					printf("\n End of playing time \n");
					stop(player);
					
				}
			}
		   
		}
		if(!player->play)
			return 1;

		printf("\n Finish playing !!!!!!\n");

		
		

	   
		

		player->err = Pa_CloseStream(player->stream);
		error_check(player->err);

		free(player->buffer);
		//ao_close(dev);
		mpg123_close(player->mh);
		mpg123_delete(player->mh);
		mpg123_exit();

		Pa_Terminate();
		return 0;
}
int stop(mp3Player* player)
{
	player->play = false;
	player->err = Pa_StopStream(player->stream);
	error_check(player->err);
	player->err = Pa_CloseStream(player->stream);
	error_check(player->err);

	free(player->buffer);
		
	mpg123_close(player->mh);
	mpg123_delete(player->mh);
	mpg123_exit();

	Pa_Terminate();
	
	return 0;
}
//#endif
