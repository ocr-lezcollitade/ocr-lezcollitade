CC=gcc
CFLAGS=-Wall -Werror -Wextra -O3 -c `pkg-config --cflags sdl2 SDL2_image`
DFLAGS=-Wall -Werror -Wextra -O0 -c -g -fsanitize=address `pkg-config --cflags sdl2 SDL2_image`

LD=gcc
LDFLAGS=-lm
LDLIBS=`pkg-config --libs sdl2 SDL2_image`

ROOT_TARGET=./bin
TARGET_DIR=$(ROOT_TARGET)/Release
TARGET=$(TARGET_DIR)/Lezcollitade
LIB=$(TARGET_DIR)/Lezcollitade.a

DEBUG_TARGET_DIR=$(ROOT_TARGET)/Debug
DEBUG_TARGET=$(DEBUG_TARGET_DIR)/Lezcollitade.debug

TEST_DIR=./tests
TEST_BIN=$(TEST_DIR)/bin
TEST_BINS=$(TEST_BIN)/utils/matrices/matrix_test $(TEST_BIN)/network/function_test

SRC_DIR=./src

SRC=main.c utils/mnist/loader.c utils/matrices/matrix.c solver/solver.c network/function.c network/network.c preproc/rotate/rotate.c

OBJ_ROOT=./obj
OBJ_DIR=$(OBJ_ROOT)/Release
OBJS=$(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

DEBUG_OBJ_DIR=$(OBJ_ROOT)/Debug
DEBUG_OBJS=$(addprefix $(DEBUG_OBJ_DIR)/, $(SRC:.c=.o))

.PHONY: all prod debug clean build dbuild lib test docs

all: prod debug

prod: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(LD) $^ $(LDLIBS) -o $@ $(LDFLAGS) 

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	@mkdir -p $(DEBUG_TARGET_DIR)
	$(LD) $^ $(LDLIBS) -o $@ $(LDFLAGS)  -fsanitize=address

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

lib: $(LIB)

$(LIB): $(OBJS)
	rm -f $(LIB)
	@mkdir -p $(TARGET_DIR)
	ar -cq $(LIB) $^
	@ar -d $(LIB) $(OBJ_DIR)/main.o 

$(TEST_BIN)/%: $(TEST_DIR)/%.c $(LIB)
	@mkdir -p $(shell echo $@ | grep -Eo "(\w+/)+")
	@# vim color scheme debug "
	$(CC) -g -Wall -Wextra -Werror $< $(LIB) -o $@ -lcriterion -lm

test: clean $(LIB) $(TEST_BINS)
	for test in $(TEST_BINS);do ./$$test; done

docs:
	doxygen doxygen.conf

clean :
	rm -rf $(OBJ_ROOT)
	rm -rf $(ROOT_TARGET)
	rm -rf $(TEST_BIN)
