#CC=arm-linux-gnueabihf-gcc
CC=gcc
CFLAGS=-I. -Wall
PLAYAUDIOFLAG=-D PLAY_AUDIO
DEPS = receive_file.h sock_infra.h advertisement.h logger.h playAudio.h acpHandler.g
OBJS = audio_app.o receive_file.o advertisement.o sock_infra.o logger.o playAudio.o acpHandler.o

		
.c.o: $(DEPS)
			$(CC) -c -Wall -o $@  $< $(CFLAGS)
			
audio_app: $(OBJS)
#			$(CC) -o $@ $^ $(CFLAGS) -pthread -lrt -lasound -ljack libportaudio.a -lmpg123
			$(CC) -o $@ $^ $(CFLAGS) -pthread
			rm *.o
#			cp audio_app /home/duyphan/ftpserver/
			
.PHONY: clean

clean:
	rm audio_app



