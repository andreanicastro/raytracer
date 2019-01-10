#include <chrono>
#include <fstream>
#include <iostream>


int main(int argc, char** argv) {
    int nx = 200;
    int ny = 100;
    std::ofstream outputfile;
    outputfile.open("./result.ppm");
    outputfile << "P3\n" << nx << " " << ny << "\n255\n";
    auto render_begin = std::chrono::high_resolution_clock::now();
    for (int j = ny -1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            float r = float(i) / float(nx);
	    float g = float(j) / float(ny);
	    float b = 0.2;
	    int ir = int(255.99 * r);
	    int ig = int(255.99 * g);
	    int ib = int(255.99 * b);
	    outputfile << ir << " " << ig << " " << ib << "\n";
	}
    }        
    outputfile.close();
    auto render_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> render_elapsed = render_end - render_begin;
    std::cout << "Total rendering time: " << render_elapsed.count() << std::endl;
}
