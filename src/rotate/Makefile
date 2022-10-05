# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 -lm `pkg-config --cflags sdl2 SDL2_image` -lm
LDFLAGS = -lm
LDLIBS = -lm `pkg-config --libs sdl2 SDL2_image`

all: rotate 

SRC = rotate.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

rotate: rotate.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
