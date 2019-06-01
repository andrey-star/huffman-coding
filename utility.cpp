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

    std::ifstream src;
    std::ofstream dest;
    src.open(src_path, std::ios::binary);
    dest.open(dest_path, std::ios::binary);

    std::clock_t start = clock();
    if (!src) {
        std::cerr << "Missing input file: " << src_path << "\n";
        return -1;
    }
    if (cmd == "-c") {
        encode(src, dest);
    } else if (cmd == "-d"){
        try {
            decode(src, dest);
        } catch (std::logic_error &e) {
            std::cerr << e.what() << "\n";
            return -1;
        }
    }
    std::cerr << "Finished in: " << std::setprecision(3) << 1.0 * (clock() - start) / CLOCKS_PER_SEC << " s" << "\n";
    return 0;
}


