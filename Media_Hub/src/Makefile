#CC=arm-linux-gnueabihf-gcc
CC=gcc
CFLAGS=-I.
DEPS = receive_file.h sock_infra.h advertisement.h logger.h
OBJS = audio_app.o receive_file.o advertisement.o sock_infra.o logger.o 

		
%.o: %.c $(DEPS)
			$(CC) -c -Wall -o $@  $< $(CFLAGS)
			
audio_app: $(OBJS)
			$(CC) -o $@ $^ $(CFLAGS) -pthread
			rm *.o
#			cp audio_app /home/duyphan/ftpserver/
			
.PHONY: clean

clean:
	rm audio_app



