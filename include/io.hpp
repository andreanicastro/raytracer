#ifndef IOHPP
#define IOHPP

#include <fstream>
#include <string>

class Streamer {
    public:
        Streamer(std::string filename, int npx, int npy) {
	    outputfile_.open(filename.c_str());	
	    outputfile_ << "P3\n" << npx << " " << npy << "\n255\n";
	};

	~Streamer() {
		outputfile_.close();
	}

	void putpixel(const float r, const float g, const float b){
	    int ir = int(255.99 * r);
	    int ig = int(255.99 * g);
	    int ib = int(255.99 * b);
	    outputfile_ << ir << " " << ig << " " << ib << "\n";
	}
	
    private:
	std::ofstream outputfile_;
};

#endif // hpp
