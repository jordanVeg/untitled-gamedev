#ALLEGRO_DIR := C:/msys/mingw64
#INCLUDE_DIR := $(ALLEGRO_DIR)/include
#LOAD_DIR := $(ALLEGRO_DIR)/bin
#CFLAGS := -std=gnu99 -Wall -g -I$(INCLUDE_DIR) -Os
#LDLIBS := -L$(LOAD_DIR) -lallegro -lallegro_main -lallegro_font -lallegro_image -lallegro_audio
CFLAGS+=-std=gnu99 -Wall -g `pkg-config --cflags allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5` -Os
LDLIBS+=`pkg-config --libs allegro-5 allegro_main-5 allegro_font-5 allegro_image-5 allegro_primitives-5`
CC:=gcc
all: main
clean:
	rm *.exe
