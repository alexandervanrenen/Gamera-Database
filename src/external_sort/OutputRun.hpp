#ifndef __OutputRun
#define __OutputRun

#include <cstdint>
#include <memory>
#include <string>

namespace dbi {

class Page;
class InputRun;

class OutputRun {
public:
   OutputRun(const std::string& fileName, bool append);

   void assignPage(std::unique_ptr<Page> page);

   void add(uint64_t data);

   void flush();

   void prepareForWriting();

   std::unique_ptr<InputRun> convertToInputRun();

private:
   const std::string fileName;
   std::unique_ptr<std::ofstream> file;
   uint64_t start;
   uint64_t end;

   std::unique_ptr<Page> page;
   uint64_t positionInPage;
   uint64_t validEntries;
   bool append;

   void writePage();
};

}

#endif
