CC=arm-linux-gnueabihf-gcc
CFLAGS=-I.
DEPS = receive_file.h sock_infra.h
OBJS = audio_app.o receive_file.o sock_infra.o

		
%.o: %.c $(DEPS)
			$(CC) -c -Wall -o $@  $< $(CFLAGS)
			
audio_app: $(OBJS)
			$(CC) -o $@ $^ $(CFLAGS) -pthread
			rm *.o
			cp audio_app /home/duyphan/ftpserver/
			
.PHONY: clean

clean:
	rm audio_app



