#CC=arm-linux-gnueabihf-gcc
CC=gcc
CFLAGS=-I. -Wall
PLAYAUDIOFLAG=-D PLAY_AUDIO
DEBUGFLAG=-D DEBUG
DEPS = receive_file.h sock_infra.h advertisement.h logger.h playAudio.h acpHandler.h
OBJS = audio_app.o receive_file.o advertisement.o sock_infra.o logger.o playAudio.o acpHandler.o

		
.c.o: $(DEPS)
#			$(CC) $(CFLAGS) $(DEBUGFLAG) $(PLAYAUDIOFLAG) -g -O0 -c -o $@  $<
			$(CC) $(CFLAGS) $(DEBUGFLAG) -g -O0 -c -m32 -o $@  $<
			
audio_app: $(OBJS)
#		 	$(CC) -o $@ $^ $(CFLAGS) $(PLAYAUDIOFLAG) -pthread -lrt -lasound -ljack libportaudio.a -lmpg123
			$(CC) -o $@ $^ $(CFLAGS) $(DEBUGFLAG) -m32 -pthread
			rm *.o
#			cp audio_app /home/duyphan/ftpserver/
			
.PHONY: clean

clean:
	rm audio_app



