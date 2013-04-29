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

   // Open input stream
   void prepareForReading();
   // Allow this run to use the page
   void assignPage(std::unique_ptr<Page> page);

   // Access file
   uint64_t peekNext() const;
   uint64_t getNext();
   bool hasNext();

   // Number of entries
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
