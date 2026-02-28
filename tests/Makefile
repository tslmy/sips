CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic
TARGET = test_helpers
SRC = test_helpers.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
