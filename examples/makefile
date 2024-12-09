CFLAGS ?= -DUSE_SHAPES -DUSE_FONTS -lm -O3 -s -flto -Wall -Wextra -std=gnu99

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

all: core_basic_window core_basic_screen_manager core_input_keys

core_basic_window: core_basic_window.c ../src/fensterRL.h
	$(CC) core_basic_window.c -I../src/ -o $@ $(CFLAGS) $(LDFLAGS)

core_basic_screen_manager: core_basic_screen_manager.c ../src/fensterRL.h
	$(CC) core_basic_screen_manager.c -I../src/ -o $@ $(CFLAGS) $(LDFLAGS)

core_input_keys: core_input_keys.c ../src/fensterRL.h
	$(CC) core_input_keys.c -I../src/ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f core_basic_window core_basic_screen_manager core_input_keys
