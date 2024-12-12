CFLAGS ?= -DUSE_FONTS -DUSE_SHAPES -DUSE_SIMD -march=native -mtune=native -fno-strict-aliasing -funroll-loops -fmerge-all-constants -Ofast -ftree-vectorize -funsafe-math-optimizations -mavx2 -lm -s -flto -Wall -Wextra -std=gnu99


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

SRC_DIR = .
SRCS = $(wildcard $(SRC_DIR)/*.c)
TARGETS = $(SRCS:%.c=%)

all: $(TARGETS)

%: %.c ../src/fensterRL.h
	$(CC) $< -I../src/ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGETS)
