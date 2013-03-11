#ifndef __OutputRun
#define __OutputRun

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <list>
#include <memory>
#include <iostream>

template<class T>
struct OutputRun {
   OutputRun(const std::string& fileName, bool append)
   : fileName(fileName), positionInPage(0), validEntries(0), append(append)
   {
   }

   void assignPage(std::unique_ptr<Page<T>> page)
   {
      this->page = std::move(page);
   }

   void add(const T& data)
   {
      page->set(positionInPage++, data);
      if (positionInPage >= page->entryCount())
         writePage();
   }

   void flush()
   {
      if (positionInPage != 0)
         writePage();
      end = file->tellp();
      file = nullptr;
   }

   void prepareForWriting()
   {
      if(append)
         file = dbiu::make_unique<std::ofstream>(fileName, std::ios::binary | std::ios::app); else
         file = dbiu::make_unique<std::ofstream>(fileName, std::ios::binary | std::ios::out);
      assert(file->is_open() && file->good());
      start = file->tellp();
      positionInPage = 0;
      validEntries = 0;
   }

   std::unique_ptr<Run<T>> createRun()
   {
   	return dbiu::make_unique<Run<T>>(start, end-start, fileName);
   }

private:
   const std::string fileName;
   std::unique_ptr<std::ofstream> file;
   uint64_t start;
   uint64_t end;

   std::unique_ptr<Page<T>> page;
   uint64_t positionInPage;
   uint64_t validEntries;
   bool append;

   void writePage()
   {
      assert(file->is_open());
      assert(file->good());
      file->write(page->begin(), positionInPage * sizeof(T));
      positionInPage = 0;
      assert(file->is_open());
      assert(file->good());
   }
};

#endif
