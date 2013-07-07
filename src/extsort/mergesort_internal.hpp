#pragma once

#include <inttypes.h>
#include <fstream>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <assert.h>
#include <map>

using namespace std;

// Generates names for temporary files and opens streams with them
class TempFile {
private:
    string filename;
    int currentnum = 0;
    map<fstream*, std::string> filemap;
public:
    TempFile(string filename) {
        this->filename = filename;
    };
    string get() {
        return filename + "." + to_string(currentnum++);
    };

    fstream* getStream() {
        string s = get();
        // hack neeeded to create not-existing files
        fstream* file = new fstream(s.c_str(), ios::binary | ios::out | ios::trunc);
        file->close();
        file->open(s.c_str(), ios::binary | ios::in | ios::out);
        filemap.insert({file, s});
        return file;
    };

    string getFilename(fstream* stream) {
        auto it = filemap.find(stream);
        assert(it != filemap.end());
        string filename = it->second;
        filemap.erase(it);
        return filename;
    };
};

// Represents one chunk
class Chunk {
private:
    fstream* file;
    //uint64_t fileoffset;
    //uint64_t filepos;
    uint64_t chunksize;
    uint64_t pos = 0;
    char* bufferpointer = nullptr;
    uint64_t partitionsize = 0;
    uint64_t offset = 0;
    uint64_t keysize;
public:
    Chunk(fstream* file, uint64_t, uint64_t chunksize, uint64_t keysize) : chunksize(chunksize), keysize(keysize) {
        this->file = file;
        //this->fileoffset = fileoffset;
        //this->filepos = fileoffset;
        this->chunksize = chunksize;
    };
    
    void close(TempFile* tf) {
        std::string filename = tf->getFilename(file);
        file->close();
        delete file;
        remove(filename.c_str());
    };

    void setbuffer(char* pointer, uint64_t partitionsize) {
        assert(partitionsize > 0);
        bufferpointer = pointer;
        this->partitionsize = partitionsize;
        load();
    };

    char* value() {
        if (partitionsize <= 0) return nullptr;
        return bufferpointer+offset;
    }

    void next() {
        if (offset < partitionsize)
            offset += keysize;
        else
            load();
    };

    uint64_t load() {
        if (pos >= chunksize) return 0;
        file->read(bufferpointer, chunksize - pos < partitionsize ? chunksize - pos : partitionsize);
        if ((uint64_t)file->gcount() < partitionsize)
            partitionsize = file->gcount();
        pos += partitionsize;
        offset = 0;
        return partitionsize;
    };
    
    void write(char* buffer, uint64_t size) {
      file->write(buffer, size);
    };
};



