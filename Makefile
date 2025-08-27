BUILD_DIR := build
CMAKE_BUILD_TYPE := Debug

.PHONY: build run clean test valgrind configure release help

# Default target
all: build

# Configure CMake (run automatically when needed)
$(BUILD_DIR)/Makefile: CMakeLists.txt
	@echo "Configuring CMake..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE)

configure: $(BUILD_DIR)/Makefile

# Build the project
build: $(BUILD_DIR)/Makefile
	@echo "Building..."
	@cd $(BUILD_DIR) && $(MAKE) -j$(nproc)

# Run the main program
run: build
	@echo "Running..."
	@./$(BUILD_DIR)/bin/logger

# Run tests
test: build
	@echo "Running tests..."
	@cd $(BUILD_DIR) && ctest --output-on-failure

# Run tests directly (alternative)
test-run: build
	@echo "Running tests directly..."
	@./$(BUILD_DIR)/logger_tests

# Clean build artifacts
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)

# Build in release mode
release:
	@$(MAKE) CMAKE_BUILD_TYPE=Release build

# Run with valgrind
valgrind: build
	@echo "Running with valgrind..."
	@valgrind --leak-check=full --log-file=valgrind.log ./$(BUILD_DIR)/bin/logger

# Force rebuild
rebuild: clean build

# Show available targets
help:
	@echo "Available targets:"
	@echo "  build     - Build the project (default)"
	@echo "  run       - Build and run the main program"
	@echo "  test      - Build and run tests with ctest"
	@echo "  test-run  - Build and run tests directly"
	@echo "  clean     - Remove build directory"
	@echo "  release   - Build in release mode"
	@echo "  valgrind  - Run with valgrind"
	@echo "  rebuild   - Clean and build"
	@echo "  configure - Force CMake reconfiguration"
	@echo "  help      - Show this help"