#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <vector>
#include "huffman.cpp"

int main(int argc, char **argv) {
    if (argc != 4 || (std::string(argv[1]) != "-c" && std::string(argv[1]) != "-d")) {
        std::cout << "Usage: -c/-d <input-file> <output-file>" << std::endl;
        return -1;
    }

    std::string cmd = argv[1];
    std::string src_path = argv[2];
    std::string dest_path = argv[3];

    std::ifstream source;
    std::ofstream destination;
    source.open(src_path, std::ios::binary);
    destination.open(dest_path, std::ios::binary);

    std::clock_t start = clock();
    if (cmd == "-c") {
        encode(source, destination);
    } else if (cmd == "-d"){
        try {
            decode(source, destination);
        }catch (std::logic_error &e) {
            std::cerr << e.what() << "\n";
        }
    }

    std::cerr << std::setprecision(3) << 1.0 * (clock() - start) / CLOCKS_PER_SEC << " s" << "\n";
}


