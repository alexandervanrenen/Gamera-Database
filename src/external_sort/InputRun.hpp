#ifndef __InputRun
#define __InputRun

#include <memory>
#include <string>
#include <cstdint>

namespace dbi {

class Page;

class InputRun {
public:
   InputRun(int64_t start, int64_t bytes, const std::string& fileName);

   void assignPage(std::unique_ptr<Page> page);

   bool hasNext();

   uint64_t peekNext() const;

   uint64_t getNext();

   void prepareForReading();

   uint64_t size() const;

private:
   const std::string fileName;
   const int64_t start;
   const int64_t end;
   uint64_t positionInFile;
   std::unique_ptr<std::ifstream> file;

   std::unique_ptr<Page> page;
   uint64_t positionInPage;
   uint64_t validEntries;

   void loadNextPage();
};

}

#endif
