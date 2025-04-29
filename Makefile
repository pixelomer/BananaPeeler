a.out: main.cc $(wildcard src/acs/*.hpp) $(wildcard src/*.hpp)
	c++ -O3 -flto -Wall -Wextra -pedantic main.cc
