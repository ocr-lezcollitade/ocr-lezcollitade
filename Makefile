CC=gcc
CFLAGS=-Wall -Werror -Wextra -O3 -c
DFLAGS=-Wall -Werror -Wextra -O0 -c -g

LD=gcc
LFLAGS=

ROOT_TARGET=./bin
TARGET_DIR=$(ROOT_TARGET)/Release
TARGET=$(TARGET_DIR)/Lezcollitade

DEBUG_TARGET_DIR=$(ROOT_TARGET)/Debug
DEBUG_TARGET=$(DEBUG_TARGET_DIR)/Lezcollitade.debug

SRC_DIR=./src
SRC=main.c solver/solver.c


OBJ_ROOT=./obj
OBJ_DIR=$(OBJ_ROOT)/Release
OBJS=$(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

DEBUG_OBJ_DIR=$(OBJ_ROOT)/Debug
DEBUG_OBJS=$(addprefix $(DEBUG_OBJ_DIR)/, $(SRC:.c=.o))

.PHONY: all prod debug clean build dbuild

all: prod debug

prod: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(LD)  $^ -o $@ $(LDFLAGS) 

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	@mkdir -p $(DEBUG_TARGET_DIR)
	$(LD)  $^ -o $@ $(LDFLAGS) 

dbuild: $(DEBUG_OBJS)

build: $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(shell echo $@ | grep -Eo "(\w+/)+")
	@# vim color scheme debug "
	$(CC) $(CFLAGS) $< -o $@

$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(shell echo $@ | grep -Eo "(\w+/)+")
	@# vim color scheme debug "
	$(CC) $(DFLAGS) $< -o $@

clean:
	rm -rf $(OBJ_ROOT)
	rm -rf $(ROOT_TARGET)
