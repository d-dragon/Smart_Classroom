#include "playAudio.h"
#include "logger.h"
#include "FileHandler.h"
#include "acpHandler.h"

#ifdef AUDIO_ENABLE
// Play function use to play a mp3Player instance
// To use this function, must have one mp3Player to call play
// In this function, create a stream to write and play mp3 file
// How to use: 
// Ex: 	
//		mp3Player* player;
//		play(player);
int play(mp3Player* player)
{
	player->play = true;

	player->mherr=0;
	printf("\n Open:%s\n",player->fileName);

	mpg123_init();
	player->mh = mpg123_new(NULL, &(player->mherr));
	player->buffer_size = mpg123_outblock(player->mh);
	player->buffer = (unsigned char*) malloc((player->buffer_size) * sizeof(unsigned char));

	/* open the file and get the decoding format */
	mpg123_open(player->mh, player->fileName);
	mpg123_getformat(player->mh, &(player->rate), &(player->channels), &(player->encoding));

	printf("\n%d encoding %d samplerate %d channels\n", player->encoding,
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

	appLog(LOG_DEBUG,"checking error");
	player->err = Pa_OpenStream(&(player->stream), NULL, &(player->out_param), player->rate,
			paFramesPerBufferUnspecified, paClipOff,NULL, NULL);
	error_check(player->err);

	player->err = Pa_StartStream(player->stream);
	error_check(player->err);

	//err = Pa_SetStreamFinishedCallback(stream, &end_cb);
	//error_check(err);

	/* decode and play */

	if(g_audio_flag == AUDIO_STOP) {
		appLog(LOG_DEBUG,"\n Playing %s .....",player->fileName);
		pthread_mutex_lock(&g_audio_status_mutex);
		g_audio_flag = AUDIO_PLAY;
		pthread_mutex_unlock(&g_audio_status_mutex);
	}

	while (player->play)
	{

		while(g_audio_flag == AUDIO_PAUSE) {
//			appLog(LOG_DEBUG,"\n Pausing %s .....",player->fileName);
			usleep(10000);
		}

		//check stop flag
		if(g_audio_flag == AUDIO_STOP) {
			break;
		}
		if(mpg123_read(player->mh, player->buffer, player->buffer_size, &player->done) == MPG123_OK)
		{
			Pa_WriteStream(player->stream, player->buffer,(player->done)/4);
		} else
		{

			/*			pthread_mutex_lock(&g_audio_status_mutex);
			 g_audio_flag = AUDIO_ERROR; //audio monitor will notify this play audio error
			 pthread_mutex_unlock(&g_audio_status_mutex);*/
			pthread_mutex_lock(&g_audio_status_mutex);
			g_audio_flag = AUDIO_STOP;
			pthread_mutex_unlock(&g_audio_status_mutex);
			break;
		}
	}
	//return for stop to release memory
	if(!player->play)
	return 1;

	player->err = Pa_StopStream(player->stream);
	error_check(player->err);

	player->err = Pa_CloseStream(player->stream);
	error_check(player->err);

	free(player->buffer);
	//ao_close(dev);
	mpg123_close(player->mh);
	mpg123_delete(player->mh);
	mpg123_exit();

	Pa_Terminate();
	appLog(LOG_DEBUG,"\n Finish playing !!!!!!\n");
	return 0;
}

// stop function will end stream and close portaudio and mpg123 
// when mp3 is playing, call stop() and as play(), its only argument is a mp3Player object
// How to use:
// Ex:
// mp3Player* player;
// play(player);
// stop(player);
int stop(mp3Player* player)
{
	printf("\n End of playing \n");
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
#endif

void *playAudioThread(void *arg) {

	int status;
	appLog(LOG_DEBUG, "inside playAudioThread..........");
	char *filename = arg;

//	appLog(LOG_DEBUG, "address filename: %p||address arg: %p", filename, arg);
	char *FilePath;
	FilePath = malloc(FILE_PATH_LEN_MAX);
	if (!FilePath) {
		appLog(LOG_DEBUG, "allocated memory failed, thread %d exited",
				(int)pthread_self());
		/*notify to client*/
		pthread_exit(NULL);
	}
	memset(FilePath, 0, FILE_PATH_LEN_MAX);
//	g_audio_flag = 0;//flag for stop play
	appLog(LOG_DEBUG, "File to play: %s", filename);
	strcat(FilePath, (char *) DEFAULT_PATH);
	strcat(FilePath, filename);

	appLog(LOG_DEBUG, "File Path: %s", FilePath);

#ifdef AUDIO_ENABLE
	mp3Player *player = malloc(sizeof(mp3Player));
	player->fileName = malloc(1024 * sizeof(char));

	player->fileName = FilePath;
	status = play(player);
	free(player);
#else
	g_audio_flag = AUDIO_PLAY;
	appLog(LOG_DEBUG, "pi is playing %s", filename);
#endif
	appLog(LOG_DEBUG, "deallocating memory");
	memset(g_file_name_playing, 0x00,128);
	free(arg);
	free(FilePath);
	appLog(LOG_DEBUG, "exit playAudioThread..........");
	pthread_exit(NULL);
}

int initAudioPlayer(char *filename) {

	/*FileInfo *file;
	 file = malloc(sizeof(FileInfo));
	 file->filename = malloc(100);
	 file->filename = "m.mp3";
	 file->index = 0;*/
	appLog(LOG_DEBUG, "inside initAudio")
	char *FileName;
	FileName = calloc(FILE_NAME_MAX, sizeof(char));
	if (FileName == NULL) {
		appLog(LOG_DEBUG, "allocated memory failed");
		return ACP_FAILED;
	}
//	appLog(LOG_DEBUG, "address FileName: %p", FileName);
//	memset(FileName, 0, FILE_NAME_MAX);
	//  strlen -1 to truncate '|' charater at end of string
	appLog(LOG_DEBUG, "debug----");
	strncat(FileName, filename, strlen(filename)); //cann't assign FileName = "m.mp3", it change pointer address -> can't free()
	appLog(LOG_DEBUG, "debug----");
	/*Need to parse file index to get file name*/

	//init play audio thread
	pthread_mutex_lock(&g_audio_status_mutex);
	g_audio_flag = AUDIO_STOP;
	pthread_mutex_unlock(&g_audio_status_mutex);

	//FileName will be freed in playAudioThread
	if (pthread_create(&g_play_audio_thd, NULL, &playAudioThread,
			(void *) FileName)) {
		appLog(LOG_DEBUG, "init playAudioThread failed!!!");
		return ACP_FAILED;
	}
	return ACP_SUCCESS;
}

int stopAudio(char *message) {

	char *resp_for;
	char *msg_id;

	msg_id = getXmlElementByName(message, "id");
	resp_for = getXmlElementByName(message, "command");

	pthread_mutex_lock(&g_audio_status_mutex);
	g_audio_flag = AUDIO_STOP;
	appLog(LOG_DEBUG, "setting flag to AUDIO_STOP");
	pthread_mutex_unlock(&g_audio_status_mutex);
	memset(g_file_name_playing,0x00,128);
	sendResultResponse(msg_id, resp_for, ACP_SUCCESS, NULL);
	return ACP_SUCCESS;
}

int pauseAudio(char *message) {

	char *resp_for;
	char *msg_id;

	msg_id = getXmlElementByName(message, "id");
	resp_for = getXmlElementByName(message, "command");

	pthread_mutex_lock(&g_audio_status_mutex);
	appLog(LOG_DEBUG, "setting flag to AUDIO_PAUSE");
	if (g_audio_flag == AUDIO_PLAY) {
		g_audio_flag = AUDIO_PAUSE;
	}
	pthread_mutex_unlock(&g_audio_status_mutex);
	sendResultResponse(msg_id, resp_for, ACP_SUCCESS, NULL);
	return ACP_SUCCESS;
}
//#endif
