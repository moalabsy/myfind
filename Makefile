# Variables to control Makefile operation
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g  -Wextra
# ****************************************************

all: myfind

myfind: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o myfind

clean:
	rm -f myfind
