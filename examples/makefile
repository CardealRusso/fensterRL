CFLAGS ?= -DUSE_FONTS -lm -O3 -s -flto -Wall -Wextra -std=gnu99

ifeq ($(OS),Windows_NT)
	CC = gcc
	LDFLAGS = -lgdi32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS = -framework Cocoa
	else
		LDFLAGS = -lX11
	endif
endif

all: 001_core_basic_window

001_core_basic_window: 001_core_basic_window.c ../src/fensterRL.h
	$(CC) 001_core_basic_window.c -I../src/ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f 001_core_basic_window
