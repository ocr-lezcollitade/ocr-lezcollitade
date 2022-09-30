# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image`

all: line_detection

SRC = line_detection.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

line_detection: line_detection.o
	$(CC) line_detection.o `pkg-config --libs sdl2 SDL2_image` -o line_detection -lm

line_detection.o: line_detection.c
	$(CC) $(CFLAGS) -c line_detection.c

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END

