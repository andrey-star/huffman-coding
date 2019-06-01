#ifndef HUFFMANCODING_BUFFERED_READER_H
#define HUFFMANCODING_BUFFERED_READER_H

#include <istream>

#include <buffered_io/CONSTANTS.h>

class buffered_reader {
public:
    explicit buffered_reader(std::istream &stream);

    bool read_char(unsigned char &x);

    void reset();

private:
    void fill_buffer();

    char buffer[BUFFER_SIZE];
    std::streamsize pos, size;
    std::istream &stream;

};

#endif //HUFFMANCODING_BUFFERED_READER_H
