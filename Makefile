#ALLEGRO_DIR := C:/Users/jorda/.nuget/packages/allegro/5.2.7.1/build/native
#INCLUDE_DIR := $(ALLEGRO_DIR)/include
#LOAD_DIR := $(ALLEGRO_DIR)/v143/x64/lib
#CFLAGS := -I$(INCLUDE_DIR) -Os -W
#LDLIBS := -L$(LOAD_DIR) -lallegro-5 -lallegro_main-5 -lallegro_font-5
CFLAGS+= -std=gnu99 `pkg-config --cflags allegro-5 allegro_main-5 allegro_font-5` -Os -W
LDLIBS+=`pkg-config --libs allegro-5 allegro_main-5 allegro_font-5`
CC:=gcc
all: hello