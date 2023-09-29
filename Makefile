CXX = clang++
CXXFLAGS = -std=c++20
BUILD_DIR = build
TARGET = $(BUILD_DIR)/apex-sim
SRC = main.cpp

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
