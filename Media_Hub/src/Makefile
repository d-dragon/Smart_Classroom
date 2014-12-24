#CC=arm-linux-gnueabihf-gcc
CC=gcc
CFLAGS=-I. -Wall
PLAYAUDIOFLAG=-D PLAY_AUDIO
DEBUGFLAG=-D DEBUG
TARGET = x86
DEPS = FileHandler.h sock_infra.h advertisement.h logger.h playAudio.h acpHandler.h
OBJS = audio_app.o FileHandler.o advertisement.o sock_infra.o logger.o playAudio.o acpHandler.o

		
.c.o: $(DEPS)
ifeq ($(TARGET),x86)
	$(CC) $(CFLAGS) $(DEBUGFLAG) -I/usr/include/python2.7 -g -O0 -c -o $@  $<
else
	$(CC) $(CFLAGS) $(DEBUGFLAG) $(PLAYAUDIOFLAG) -I/usr/include/python2.7 -g -O0 -c -o $@  $<
endif
			
audio_app: $(OBJS)
ifeq ($(TARGET),x86)
	$(CC) -o $@ $^ $(CFLAGS) $(DEBUGFLAG) -pthread -lpython2.7
else
 	$(CC) -o $@ $^ $(CFLAGS) $(PLAYAUDIOFLAG) -pthread -lrt -lasound -ljack -lpython2.7 libportaudio.a -lmpg123
endif
			
			rm *.o
#			cp audio_app /home/duyphan/ftpserver/
			
.PHONY: clean

clean:
	rm audio_app



