#include "InputRun.hpp"
#include "Page.hpp"
#include "util/Utility.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iostream>
#include <list>
#include <stdio.h>

namespace dbi {

InputRun::InputRun(int64_t start, int64_t bytes, const std::string& fileName)
: fileName(fileName), start(start), end(start + bytes), positionInFile(start), positionInPage(0), validEntries(0)
{
}

void InputRun::assignPage(std::unique_ptr<Page> page)
{
   this->page = std::move(page);
}

bool InputRun::hasNext()
{
   if(positionInPage >= validEntries)
      loadNextPage();
   return positionInPage < validEntries;
}

uint64_t InputRun::peekNext() const
{
   assert(positionInPage < validEntries);
   return page->get(positionInPage);
}

uint64_t InputRun::getNext()
{
   assert(hasNext());
   return page->get(positionInPage++);
}

void InputRun::prepareForReading()
{
   file = util::make_unique<std::ifstream>(fileName, std::ios::binary | std::ios::in);
   assert(file->is_open() && file->good());
   assert(positionInPage == 0);
   assert(validEntries == 0);
   loadNextPage();
}

uint64_t InputRun::size() const
{
   return end - start;
}

void InputRun::loadNextPage()
{
   // Check if there is more to read
   assert(file->is_open());
   assert(file->good());
   uint64_t validBytes = std::min(page->size(), end - positionInFile);
   if(validBytes <= 0)
      return;

   // Read
   file->seekg(positionInFile, std::ios::beg);
   file->read(page->begin(), validBytes);
   validEntries = validBytes / sizeof(uint64_t);
   positionInPage = 0;
   positionInFile += validBytes;
   assert(file->is_open());
   assert(file->good());
}

}
