#pragma once

#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include "common/Config.hpp"
#include "SortIterator.hpp"
#include "btree/IndexKeyComparator.hpp"
#include "btree/IndexKeySchema.hpp"
#include "btree/IndexKey.hpp" 
#include "mergesort_internal.hpp"

namespace dbi {


class MergeSort {
private:
    IndexKeySchema& schema;
    IndexKeyComparator& c;
    uint64_t bytes;
    uint64_t memsize;
    uint64_t pagesize;
    char* buffer;
    TempFile* tf;
public:
    MergeSort(uint64_t pagesize, uint64_t memsize, IndexKeySchema schema, IndexKeyComparator c) : schema(schema), c(c), bytes(schema.bytes()), memsize(memsize - memsize%schema.bytes()), pagesize(pagesize) {
        assert(memsize % pagesize == 0);
        buffer = new char[memsize];
    }

    ~MergeSort() {
        delete[] buffer;
    }

    void sortchunk(fstream *file, uint64_t pos) {
        SortIterator ibuf = SortIterator(buffer, bytes);
        SortIterator ibufend = SortIterator(buffer+memsize-bytes, bytes);
        std::cout << "Reading data\n";
        file->seekg(pos);
        file->read(buffer, memsize);
        std::cout << "Sorting data\n";
        dbi::sort(ibuf, ibufend, c);
        std::cout << "Writing data\n";
        file->seekp(pos);
        file->write(buffer, memsize);
    }
    
    Chunk* mergesingle(fstream* outfile, vector<Chunk*> chunks) {
        assert(chunks.size() * pagesize <= (memsize-pagesize));
        int i = 0;
        // assign each chunk a part of the buffer
        for (auto chunk : chunks) {
            chunk->setbuffer(buffer+pagesize*i, pagesize);
            i++;
        }
        // prepare output buffer
        char* outbuf = buffer+pagesize*i;//new char[PAGESIZE];
        char* saveoutbuf = outbuf;

        uint64_t size = 0;
        while(true) {
            int i = 0;
            int minindex = 0;
            // find smallest value in currently loaded chunk parts
            for (auto chunk : chunks) {
                if (chunk->value() != nullptr && c.less(chunk->value(), chunks[minindex]->value())) {
                    minindex = i;
                }
                i++;
            }
            if (chunks[minindex]->value() == nullptr) break; // no more values
            std::memcpy(outbuf, chunks[minindex]->value(), bytes);
            chunks[minindex]->next();
            outbuf+= bytes;
            size+= bytes;
            // if output buffer is full, write it to disk
            if (outbuf >= saveoutbuf+pagesize) {
                outfile->write(saveoutbuf, pagesize);
                outbuf = saveoutbuf;
            }
        }
        // if output buffer is not empty, write it to disk
        if (outbuf != saveoutbuf) {
            outfile->write(saveoutbuf, (char*)outbuf - saveoutbuf);
        }
        outfile->flush();
        outfile->seekg(0, outfile->beg);
        return new Chunk(outfile, 0, size, bytes);
    }

    Chunk* mergechunks(fstream* out, vector<Chunk*> chunks) {
        vector<Chunk*> secondrun;
        // Merge as many chunks as possible at once 
        while ((memsize-pagesize) / chunks.size() < pagesize && chunks.size() > 1) {
            secondrun.push_back(chunks.back());
            chunks.pop_back();
        }
        Chunk* mergechunk1 = mergesingle(secondrun.size() != 0 ? tf->getStream() : out, chunks);
        Chunk* mergechunk2;
        // Merge the remaining chunks and then merge the two result chunks
        if (secondrun.size() > 0) {
            mergechunk2 = mergechunks(tf->getStream(), secondrun);
            vector<Chunk*> finalrun = {mergechunk1, mergechunk2};
            Chunk* finalchunk = mergesingle(out, finalrun);
            mergechunk1->close(tf);
            mergechunk2->close(tf);
            delete mergechunk1;
            delete mergechunk2;
            return finalchunk;
        } else {
            return mergechunk1;
        }
    }

    int externalsort(string in, string out) {
        std::cout << "Starting externalsort\n";
        struct stat st;
        if (stat(in.c_str(), &st) != 0)
            return 1;
        uint length = st.st_size;

        fstream infile;
        infile.open(in.c_str(), ios::binary | ios::in | ios::out);
        if (!infile.is_open()) // io error 
            return 1;

        fstream outfile;
        outfile.open(out.c_str(), ios::binary | ios::out);
        if (!outfile.is_open())
            return 1;
        vector<Chunk*> chunks;
        std::cout << "Sorting chunks\n";
        // split input file into chunks and sort them individually
        for (uint64_t i = 0; i < length; i+=memsize) {
            sortchunk(&infile, i);
            std::cout << "Sorted chunk " << i << std::endl;
            chunks.push_back(new Chunk(&infile, i, memsize, bytes));
        }

        tf = new TempFile(out);
        // do a merge on the chunks
        std::cout << "Merging chunks\n";
        Chunk* finalchunk = mergechunks(&outfile, chunks);
        // cleanup
        infile.close();
        for (auto chunk: chunks) delete chunk;
        outfile.flush();
        outfile.close();
        delete finalchunk;
        delete tf;
        return 0;
    }
    /*
    bool checksort(const char* filename) {
        ifstream file;
        file.open(filename, ios::binary | ios::in);
        file.seekg (0, file.end);
        int length = file.tellg();
        file.seekg (0, file.beg);
        bool result = checkchunks(&file, length);
        file.close();
        return result;
    }
    
    bool checkchunk(uint64_t& maxval) {
        if (chunk[0] < maxval)
            return false;
        for (uint64_t i=0; i < numsinchunk-1; i++) {
            if (chunk[i] > chunk[i+1])
                return false;
        }
        maxval = chunk[numsinchunk-1];
        return true;
    }

    bool checkchunks(ifstream* file, uint64_t length) {
        IndexKey maxval = 0;
        for (uint64_t i=0; i < (length/memsize); i++) {
            memset(buffer, 0, memsize);
            file->read(buffer, memsize);
            if (!checkchunk(maxval))
                return false;
        }
        return true;
    }

    */
};

}
