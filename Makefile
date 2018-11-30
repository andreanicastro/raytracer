all: main.cpp vec.hpp ray.hpp
	g++ -std=c++11 -O3 -o bin/raytrace  main.cpp -I.
	g++ -std=c++11 -g -o bin/raytrace_debug main.cpp -I.
