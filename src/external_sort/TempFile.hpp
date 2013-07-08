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

namespace dbi {

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

    ~TempFile() {
        for (auto p : filemap) {
            p.first->close();
            delete p.first;
        }
    }
    
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


}
