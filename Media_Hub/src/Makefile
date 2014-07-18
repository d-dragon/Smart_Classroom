CC=gcc
CFLAGS=-I.
DEPS = receive_file.h sock_infra.h
OBJS = audio_app.o receive_file.o sock_infra.o

		
%.o: %.c $(DEPS)
			$(CC) -c -Wall -o $@  $< $(CFLAGS) -pthread
			
audio_app: $(OBJS)
			$(CC) -o $@ $^ $(CFLAGS) -pthread
			
.PHONY: clean

clean:
	rm audio_app *.o


