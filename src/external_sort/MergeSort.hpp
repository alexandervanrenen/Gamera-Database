#pragma once

#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include "common/Config.hpp"
#include "SortIterator.hpp"
#include "btree/IndexKeyComparator.hpp"
#include "btree/IndexKeySchema.hpp"
#include "btree/IndexKey.hpp" 
#include "TempFile.hpp"
#include "Chunk.hpp"

namespace dbi {


uint64_t filesize(fstream* f) {
    f->seekg(0, f->end);
    uint64_t length = f->tellg();
    f->seekg(0, f->beg);
    return length;
}

class MergeSort {
private:
    IndexKeySchema schema;
    IndexKeyComparator c;
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

    Chunk* sortchunk(fstream *infile, fstream* outfile, uint64_t pos, uint64_t bufsize) {
        assert(bufsize > 0 && bufsize <= memsize);
        SortIterator ibuf = SortIterator(buffer, bytes);
        SortIterator ibufend = SortIterator(buffer+bufsize-bytes, bytes);
        infile->seekg(pos);
        infile->read(buffer, bufsize);
        assert((uint64_t)infile->gcount() == bufsize);
        dbi::sort(ibuf, ibufend, c, bytes);
        // For testing
        char* bufp1 = buffer;
        char* bufp2 = buffer+bytes;
        for (uint64_t i=bytes; i < bufsize; i+=bytes) {
            //std::cout << *((int32_t*)bufp2) << std::endl;
            assert(c.less(bufp1, bufp2));
            bufp1 += bytes;
            bufp2 += bytes;
        }
        
        outfile->seekp(0);
        outfile->write(buffer, bufsize);
        return new Chunk{outfile, 0, bufsize, bytes};
    }
    
    Chunk* mergesingle(fstream* outfile, vector<Chunk*> chunks) {
        //std::cout << "Mergesingle, pagesize: " << pagesize << std::endl;
        assert(chunks.size() * pagesize <= (memsize-pagesize));
        int i = 0;
        // assign each chunk a part of the buffer
        for (auto chunk : chunks) {
            //std::cout << "Chunk " << i << " setting pagesize " << pagesize << std::endl;
            chunk->setbuffer(buffer+pagesize*i, pagesize);
            i++;
        }
        // prepare output buffer
        char* outbuf = buffer+pagesize*i;//new char[PAGESIZE];
        char* saveoutbuf = outbuf;

        uint64_t size = 0;
        char* minval = new char[bytes];
        bool invalid = false;
        std::memcpy(minval, chunks[0]->value(), bytes);
        while(true) {
            int i = 0;
            int minindex = 0;
            // find smallest value in currently loaded chunk parts
            for (auto chunk : chunks) {
                if (chunk->value() != nullptr && (c.less(chunk->value(), minval) || invalid)) {
                    invalid = false;
                    minindex = i;
                    std::memcpy(minval, chunk->value(), bytes);
                }
                //std::cout << *((uint32_t*)chunk->value()) << std::endl;
                //if (chunk->value() != nullptr && chunks[minindex]->value() != nullptr && c.less(chunk->value(), chunks[minindex]->value())) {
                //    minindex = i;
                //}
                i++;
            }
            if (chunks[minindex]->value() == nullptr || invalid) break; // no more values
            //std::memcpy(outbuf, chunks[minindex]->value(), bytes);
            std::memcpy(outbuf, minval, bytes);
            invalid = true;
            //std::cout << *((int32_t*)minval) << std::endl;
            chunks[minindex]->next();
            outbuf += bytes;
            size += bytes;
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
        //std::cout << "Size of outfile: " << filesize(outfile) << std::endl;
        outfile->seekg(0, outfile->beg);
        assert(size > 0);
        delete[] minval;
        return new Chunk(outfile, 0, size, bytes);
    }

    Chunk* mergechunks(fstream* out, vector<Chunk*> chunks) {
        //std::cout << "mergechunks aufruf\n";
        vector<Chunk*> secondrun;
        // Merge as many chunks as possible at once 
        while ((memsize-pagesize) / chunks.size() < pagesize && chunks.size() > 1) {
            secondrun.push_back(chunks.back());
            chunks.pop_back();
        }
        assert(chunks.size() > 1);
        //std::cout << "Chunks in first run: " << chunks.size() << std::endl;
        //std::cout << "Chunks in second run: " << secondrun.size() << std::endl;
        Chunk* mergechunk1 = mergesingle(secondrun.size() != 0 ? tf->getStream() : out, chunks);
        Chunk* mergechunk2;
        // Merge the remaining chunks and then merge the two result chunks
        if (secondrun.size() > 0) {
            if (secondrun.size() > 1) 
                mergechunk2 = mergechunks(tf->getStream(), secondrun);
            else
                mergechunk2 = secondrun.back();
            vector<Chunk*> finalrun = {mergechunk1, mergechunk2};
            Chunk* finalchunk = mergesingle(out, finalrun);
            mergechunk1->close(tf);
            mergechunk2->close(tf);
            delete mergechunk1;
            if (secondrun.size() != 1) delete mergechunk2;
            return finalchunk;
        } else {
            return mergechunk1;
        }
    }

    int externalsort(string in, string out) {
        tf = new TempFile(out);
        struct stat st;
        if (stat(in.c_str(), &st) != 0)
            return 1;
        uint length = st.st_size;
        //std::cout << "Length of file: " << length << ", buffersize: " << memsize  <<  std::endl;

        fstream infile;
        fstream* infilep = new fstream();
        infile.open(in.c_str(), ios::binary | ios::in | ios::out);
        if (!infile.is_open()) // io error 
            return 1;
        infilep->open(in.c_str(), ios::binary | ios::in | ios::out);
        assert(infilep->is_open());
        //infile.exceptions ( std::ifstream::failbit | std::ifstream::badbit );
        fstream outfile;
        outfile.open(out.c_str(), ios::binary | ios::out);
        if (!outfile.is_open())
            return 1;
        vector<Chunk*> chunks;
        Chunk* finalchunk;
        // Entire file fits into memory
        if (length <= memsize) {
            finalchunk = sortchunk(&infile, &outfile, 0, length);

        } else {
            // split input file into chunks and sort them individually
            for (uint64_t i = 0; i < length; i+=memsize) {
                chunks.push_back(sortchunk(&infile, tf->getStream(), i, i+memsize > length ? length - i: memsize));
            }
            // do a merge on the chunks
            finalchunk = mergechunks(&outfile, chunks);
        }
        // cleanup
        infile.close();
        infilep->close();
        delete infilep;
        for (auto chunk: chunks) delete chunk;
        outfile.flush();
        outfile.close();
        delete finalchunk;
        delete tf;
        return 0;
    }
    
    bool checksort(const char* filename) {
        ifstream file;
        file.open(filename, ios::binary | ios::in);
        file.seekg (0, file.end);
        uint64_t length = file.tellg();
        //std::cout << "Length of file: " << length << std::endl;
        file.seekg (0, file.beg);
        char* buf = new char[length];
        file.read(buf, length);
        char* bufp1 = buf;
        char* bufp2 = buf+bytes;
        for (uint64_t i=bytes; i < length; i+=bytes) {
            //std::cout << *((uint32_t*)bufp2) << std::endl;
            if (c.less(bufp2, bufp1)) {
                //std::cout << *((int32_t*)bufp1) << std::endl;
                //std::cout << *((int32_t*)bufp2) << std::endl;
                //std::cout << "Wrong number at " << i << std::endl;
                return false;
            }
            bufp1 += bytes;
            bufp2 += bytes;
        }
        delete[] buf;
        file.close();
        return true;
    }
    

};

}
