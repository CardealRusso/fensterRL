CFLAGS ?= -O3 -s -flto -Wall -Wextra -std=gnu99

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

all: whitenoise

whitenoise: whitenoise.c ../src/fensterRL.h
	$(CC) whitenoise.c -I../src/ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f whitenoise
