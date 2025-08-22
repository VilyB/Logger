CC := g++
CXXFLAGS := -std=c++17 -Wall -Wfatal-errors

# System SDL2 via pkg-config
PKG_CFLAGS := $(shell pkg-config --cflags sdl2)
PKG_LIBS   := $(shell pkg-config --libs sdl2)

# ImGui paths (submodule)
IMGUI_DIR := ./libraries/imgui

INCLUDES := -Isrc/core -Isrc/io -Isrc/net -Isrc/ui -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends $(PKG_CFLAGS)
SOURCES  := ./src/*.cpp \
            $(IMGUI_DIR)/imgui.cpp \
            $(IMGUI_DIR)/imgui_draw.cpp \
            $(IMGUI_DIR)/imgui_tables.cpp \
            $(IMGUI_DIR)/imgui_widgets.cpp \
            $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp \
            $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp

BIN := bin/logger

.PHONY: build run clean

build:
	@echo "building..."
	@$(CC) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(BIN) $(PKG_LIBS)

run:
	@echo "running..."
	@./$(BIN)

clean:
	@echo "cleaning..."
	@rm -f $(BIN)

valgrind:
	@echo "running with valgrind..."
	@valgrind --leak-check=full --log-file=valgrind.log ./bin/logger
