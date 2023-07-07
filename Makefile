CC = gcc
CFLAGS = -Wall

all: data_av

data_av: data_av.c
	$(CC) data_av.c $(CFLAGS) -lpthread -o data_av -g
clean: 
	rm -f data_av
