CXX = clang++
CXXFLAGS = -std=c++20
BUILD_DIR = build
INPUT_FILES_DIR = input
INPUT_FILE = input.asm
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
	rm -rf $(BUILD_DIR)
