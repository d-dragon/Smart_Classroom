#CC=arm-linux-gnueabihf-gcc
CC=gcc
CFLAGS=-I. -Wall
PLAYAUDIOFLAG=-D PLAY_AUDIO
DEBUGFLAG=-D DEBUG
DEPS = FileHandler.h sock_infra.h advertisement.h logger.h playAudio.h acpHandler.h
OBJS = audio_app.o FileHandler.o advertisement.o sock_infra.o logger.o playAudio.o acpHandler.o

		
.c.o: $(DEPS)
#			$(CC) $(CFLAGS) $(DEBUGFLAG) $(PLAYAUDIOFLAG) -g -O0 -c -o $@  $<
			$(CC) $(CFLAGS) $(DEBUGFLAG) -I/usr/include/python2.7 -g -O0 -c -o $@  $<
			
audio_app: $(OBJS)
#		 	$(CC) -o $@ $^ $(CFLAGS) $(PLAYAUDIOFLAG) -pthread -lrt -lasound -ljack libportaudio.a -lmpg123
			$(CC) -o $@ $^ $(CFLAGS) $(DEBUGFLAG) -pthread -lpython2.7
			rm *.o
#			cp audio_app /home/duyphan/ftpserver/
			
.PHONY: clean

clean:
	rm audio_app



