#CC=arm-linux-gnueabihf-gcc
CC=gcc
CFLAGS=-I. -Wall
PLAYAUDIOFLAG=-D AUDIO_ENABLE
DEBUGFLAG=-D DEBUG
TARGET = x86
DEPS = FileHandler.h sock_infra.h advertisement.h logger.h playAudio.h acpHandler.h xmlHandler.h
OBJS = audio_app.o FileHandler.o advertisement.o sock_infra.o logger.o playAudio.o acpHandler.o xmlHandler.o

		
.c.o: $(DEPS)
ifeq ($(TARGET),x86)
	$(CC) $(CFLAGS) $(DEBUGFLAG) -I/usr/include/python2.7 -I/usr/include/libxml2 -g -O0 -c -o $@  $<
else
	$(CC) $(CFLAGS) $(DEBUGFLAG) $(PLAYAUDIOFLAG) -D RPI -I/usr/include/python2.7 -I/usr/include/libxml2 -g -O0 -c -o $@  $<
endif
			
audio_app: $(OBJS)
ifeq ($(TARGET),x86)
	$(CC) -o MediaHub $^ $(CFLAGS) $(DEBUGFLAG) -pthread -lpython2.7 -lxml2
else
	$(CC) -o MediaHub $^ $(CFLAGS) $(PLAYAUDIOFLAG) -D RPI -pthread -lrt -lasound -ljack -lpython2.7 -lxml2 libportaudio.a -lmpg123
endif
			
	rm *.o
	sudo rm /usr/bin/MediaHub
	sudo rm /usr/bin/startMediaHub.sh
	sudo cp MediaHub /usr/bin/
	sudo cp startMediaHub.sh /usr/bin/
			
.PHONY: clean

clean:
	rm audio_app



