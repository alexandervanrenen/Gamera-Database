#ifndef __Run
#define __Run

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <list>
#include <memory>
#include <iostream>

template<class T>
struct Run {
   Run(int64_t start, int64_t bytes, const std::string& fileName)
   : fileName(fileName), start(start), end(start+bytes), positionInFile(start), positionInPage(0), validEntries(0)
   {
   }

   int64_t getBytes()
   {
      return end - positionInFile;
   }

   void assignPage(std::unique_ptr<Page<T>> page, bool load = true)
   {
      this->page = std::move(page);
      if(load)
         loadNextPage();
   }

   bool hasNext()
   {
      if (positionInPage >= validEntries)
         loadNextPage();
      return positionInPage < validEntries;
   }

   T peekNext()
   {
      assert(hasNext());
      return page->get(positionInPage);
   }

   T getNext()
   {
      assert(hasNext());
      return page->get(positionInPage++);
   }

   void add(const T& data)
   {
      page->set(positionInPage, data);
      positionInPage++;
      if (positionInPage >= page->entryCount())
         writePage();
   }

   void flush()
   {
      if (positionInPage != 0)
         writePage();
   }

   void prepareForReading()
   {
      file = dbiu::make_unique<std::fstream>(fileName, std::ios::binary | std::ios::in);
      assert(file->is_open() && file->good());
      positionInFile = start;
      positionInPage = 0;
      validEntries = 0;
   }

   void prepareForWriting()
   {
      file = dbiu::make_unique<std::fstream>(fileName, std::ios::binary | std::ios::out);
      assert(file->is_open() && file->good());
      positionInFile = start;
      positionInPage = 0;
      validEntries = 0;
   }

   uint64_t size()
   {
      return end-start;
   }

   const std::string& getFileName()
   {
      return fileName;
   }

private:
   std::string fileName;
   const int64_t start;
   const int64_t end;
   uint64_t positionInFile;
   std::unique_ptr<std::fstream> file;

   std::unique_ptr<Page<T>> page;
   uint64_t positionInPage;
   uint64_t validEntries;

   void loadNextPage() {
      // Check if there is more to read
      assert(file->is_open());
      assert(file->good());
      uint64_t validBytes = std::min(page->size(), end - positionInFile);
      if(validBytes <= 0)
         return;

      // Read
      file->seekg(positionInFile, std::ios::beg);
      file->read(page->begin(), validBytes);
      validEntries = validBytes / sizeof(T);
      positionInPage = 0;
      positionInFile += validBytes;
      assert(file->is_open());
      assert(file->good());
   }

   void writePage()
   {
      assert(file->is_open());
      assert(file->good());
      file->seekg(positionInFile, std::ios::beg);
      file->write(page->begin(), positionInPage * sizeof(T));
      positionInFile += positionInPage * sizeof(T);
      positionInPage = 0;
      assert(file->is_open());
      assert(file->good());
   }
};

#endif
