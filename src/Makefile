INCLUDE_DIR =../include
CFLAGS+=-std=gnu99 -Wall -g `pkg-config --cflags allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5` -I$(INCLUDE_DIR)
LDLIBS+=`pkg-config --libs allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5`
CC:=gcc

DEPS = global.h collisions.h player.h
OBJS = main.o global.o collisions.o player.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LDLIBS)

main: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

all: main

.PHONY: clean

clean:
	rm *.o *.exe
