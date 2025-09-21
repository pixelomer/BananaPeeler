peel: main.cc $(wildcard src/acs/*.hpp) $(wildcard src/*.hpp) Makefile
	c++ -o peel -O3 -flto -Wall -Wextra -pedantic main.cc
