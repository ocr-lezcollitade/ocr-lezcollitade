CC=gcc

BASE_FLAGS=-Wall -Wextra -c `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image`
CFLAGS=$(BASE_FLAGS) -O3
DFLAGS=$(BASE_FLAGS) -Werror -O0 -g -fsanitize=address

LD=gcc
LDFLAGS=-lm -rdynamic
LDLIBS=`pkg-config --libs gtk+-3.0 sdl2 SDL2_image`

ROOT_TARGET=./bin
TARGET_DIR=$(ROOT_TARGET)/Release
TARGET=$(TARGET_DIR)/Lezcollitade
INSTALL_TARGET=./solver
LIB=$(TARGET_DIR)/Lezcollitade.a

DEBUG_TARGET_DIR=$(ROOT_TARGET)/Debug
DEBUG_TARGET=$(DEBUG_TARGET_DIR)/solver.debug

TEST_DIR=./tests
TEST_BIN=$(TEST_DIR)/bin
TEST_BINS=$(TEST_BIN)/utils/matrices/matrix_test\
		  $(TEST_BIN)/network/function_test

SRC_DIR=./src

SRC=main.c cli/parser.c utils/mnist/loader.c utils/matrices/matrix.c\
	solver/solver.c network/function.c network/network.c\
	preproc/rotate/rotate.c preproc/sudoku_detection/sudoku_split.c\
	preproc/color_removal/color_removal.c ui/ui.c\
	utils/img_loader/loader.c

OBJ_ROOT=./obj
OBJ_DIR=$(OBJ_ROOT)/Release
OBJS=$(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

DEBUG_OBJ_DIR=$(OBJ_ROOT)/Debug
DEBUG_OBJS=$(addprefix $(DEBUG_OBJ_DIR)/, $(SRC:.c=.o))

.PHONY: all prod debug clean build dbuild lib test docs format install

all: prod debug

install: prod
	cp $(TARGET) $(INSTALL_TARGET)

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

format:
	find ./src | grep -E "\.(c|h)$$" | xargs -I "{}" bash -c "clang-format -i {}"

clean :
	rm -rf $(OBJ_ROOT)
	rm -rf $(ROOT_TARGET)
	rm -rf $(TEST_BIN)
