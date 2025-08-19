######################################################
# Declare some Makefile Variables
######################################################
CC = g++

LANG_STD = -std=c++17

COMPILER_FLAGS = -Wall -Wfatal-errors

INCLUDE_PATH = 	-I"./libraries/imgui" \
				-I"./libraries/imgui/backends" \
				-I"./libraries/vendored/SDL/include"

SRC_FILES = ./src/*.cpp \
            ./libraries/imgui/imgui.cpp \
            ./libraries/imgui/imgui_draw.cpp \
            ./libraries/imgui/imgui_tables.cpp \
            ./libraries/imgui/imgui_widgets.cpp \
            ./libraries/imgui/backends/imgui_impl_sdl3.cpp \
            ./libraries/imgui/backends/imgui_impl_sdlrenderer3.cpp

LINKER_FLAGS = 	-L./libraries/vendored/SDL/build -lSDL3 \
				-Wl,-rpath,'$$ORIGIN/libraries/vendored/SDL/build'

OBJ_NAME = logger

######################################################
# Declare some Makefile rules
######################################################
build: clean
	@echo "building..."
	@$(CC) $(COMPILER_FLAGS) $(LANG_STD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS) -o $(OBJ_NAME)

run: 
	@echo "running..."
	@./$(OBJ_NAME)

clean:
	@echo "cleaning..."
	@rm -f $(OBJ_NAME)