CC=gcc
CFLAGS=-Wall -Werror -Wextra -O3 -c
DFLAGS=-Wall -Werror -Wextra -O0 -c -g

LD=gcc
LFLAGS=

TARGET_DIR=./bin
TARGET=$(TARGET_DIR)/Lezcollitade

DEBUG_TARGET_DIR=$(TARGET_DIR)/Debug
DEBUG_TARGET=$(DEBUG_TARGET_DIR)/Lezcollitade.debug

SRC_DIR=./src
SRC=main.c

OBJ_DIR=./obj/Release
OBJS=$(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))

DEBUG_OBJ_DIR=./obj/Debug
DEBUG_OBJS=$(addprefix $(DEBUG_OBJ_DIR)/, $(SRC:.c=.o))


prod: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(LD)  $^ -o $@ $(LDFLAGS) 

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	@mkdir -p $(DEBUG_TARGET_DIR)
	$(LD)  $^ -o $@ $(LDFLAGS) 

build_debug: $(DEBUG_OBJS)

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
	rm -rf $(DEBUG_OBJ_DIR)
	rm -rf $(OBJ_DIR)
	rm -rf $(TARGET_DIR)
