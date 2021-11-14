BUILD_DIR := bin
OBJ_DIR := obj

SRC_FILES := $(shell find src -name '*.c')
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o)
DIRECTORIES := $(shell find src -type d)

COMPILER_FLAGS := -g -MD -Werror=vla -fdeclspec -fPIC
INCLUDE_FLAGS := -Isrc
LINKER_FLAGS := -g -shared -lglfw -lvulkan -lc

all: scaffold lib test

# create directories
.PHONY: scaffold
scaffold:
	mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRECTORIES))
	mkdir -p bin

# link them
.PHONY: lib
lib: scaffold $(OBJ_FILES)
	clang $(OBJ_FILES) -o $(BUILD_DIR)/librenderer.so $(LINKER_FLAGS) 

.PHONY: test
test: lib
	clang -lrenderer -rpath ./bin -L./bin $(INCLUDE_FLAGS) test.c -o $(BUILD_DIR)/test

.PHONY: run
run: test
	./bin/test

$(OBJ_DIR)/%.c.o: %.c # compile .c to .o object
	clang -fPIC $< $(COMPILER_FLAGS) -c -o $@ $(INCLUDE_FLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(OBJ_DIR)
