CXX = clang++
CXXFLAGS = -std=c++20
BUILD_DIR = build
INPUT_FILES_DIR = input
INPUT_FILE = h1.asm
TARGET = $(BUILD_DIR)/apex-sim
SRC = main.cpp
INPUT = $(INPUT_FILES_DIR)/$(INPUT_FILE)

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET) $(INPUT)

clean:
	rm -rf $(TARGET)

clean_build_run: clean all run

# alias
cbr: clean_build_run

.PHONY: all run clean clean_build_run cbr
