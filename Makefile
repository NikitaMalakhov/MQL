CXX = g++
CXXFLAGS = -std=c++17
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./build

SRC_FILES := $(shell find $(SRC_DIR) -type f -name "*.cpp" -not -path "*/sockets/*")
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
EXECUTABLE = $(BIN_DIR)/sql

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)