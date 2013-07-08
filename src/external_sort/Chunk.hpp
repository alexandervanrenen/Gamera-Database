#pragma once

#include <inttypes.h>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <assert.h>
#include "TempFile.hpp"

using namespace std;

namespace dbi {

// Represents one chunk
class Chunk {
private:
    fstream* file;
    bool finished = false;
    const uint64_t chunksize; // size of area in file which comprises chunk (chunksize == filelength in tempfiles)
    uint64_t pos = 0; // current position in file (relative to fileoffset)
    char* bufferpointer = nullptr; // pointer in buffer to store data from chunk
    uint64_t buffersize = 0; // size of buffer to use
    uint64_t bufferoffset = 0; // current offset in buffer
    const uint64_t fileoffset; // start of area in file where chunk is saved
    const uint64_t keysize;
public:
    Chunk(fstream* file, uint64_t fileoffset, uint64_t chunksize, uint64_t keysize) : chunksize(chunksize), fileoffset(fileoffset), keysize(keysize) {
        this->file = file;
        assert(chunksize > 0);
        //std::cout << "Chunksize: " << chunksize << std::endl;
    };

    void close(TempFile* tf) {
        std::string filename = tf->getFilename(file);
        file->close();
        delete file;
        remove(filename.c_str());
    };

    void setbuffer(char* pointer, uint64_t size) {
        assert(size > 0 && size % keysize == 0);
        bufferpointer = pointer;
        buffersize = size; 
        load();
    };

    char* value() {
        if (bufferoffset >= buffersize || finished) return nullptr;
        return bufferpointer+bufferoffset;
    }

    void next() {
        if (bufferoffset+keysize < buffersize)
            bufferoffset += keysize;
        else
            load();
    };

    uint64_t load() {
        if (pos >= chunksize) {
            finished = true;
            return 0; // no more data in file
        }
        file->seekg(pos+fileoffset);
        //std::cout << "pos: " << pos << ", fileoffset: " << fileoffset << ", tellg: " << file->tellg() << std::endl;
        if (!file->good()) {
            finished = true;
        }
        //std::cout << "tellg: " << file->tellg() << std::endl; 
        file->read(bufferpointer, chunksize - pos < buffersize ? chunksize - pos : buffersize);
        if ((uint64_t)file->gcount() < buffersize)
            buffersize = file->gcount();
        //std::cout << "Read " << buffersize << " bytes\n";
        pos += buffersize;
        bufferoffset = 0;
        return buffersize;
    };
    
    void write(char* buffer, uint64_t size) {
      file->write(buffer, size);
    };
};

}

