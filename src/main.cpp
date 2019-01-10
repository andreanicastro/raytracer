#include <chrono>
#include <iostream>
#include "io.hpp"


int main(int argc, char** argv) {
    int nx = 200;
    int ny = 100;
    Streamer outputstreamer("./result.ppm", 200, 100);
    auto render_begin = std::chrono::high_resolution_clock::now();
    for (int j = ny -1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            float r = float(i) / float(nx);
	    float g = float(j) / float(ny);
	    float b = 0.2;
	    outputstreamer.putpixel(r, g, b);
	}
    }        
    auto render_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> render_elapsed = render_end - render_begin;
    std::cout << "Total rendering time: " << render_elapsed.count() << std::endl;
}
 
