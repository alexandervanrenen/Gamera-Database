#include "OutputRun.hpp"
#include "util/Utility.hpp"
#include "Page.hpp"
#include "InputRun.hpp"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include <list>
#include <iostream>

using namespace std;

namespace dbi {

OutputRun::OutputRun(const std::string& fileName, bool append)
: fileName(fileName), positionInPage(0), validEntries(0), append(append)
{
}

void OutputRun::assignPage(std::unique_ptr<Page> page)
{
   this->page = std::move(page);
}

void OutputRun::add(uint64_t data)
{
   page->set(positionInPage++, data);
   if (positionInPage >= page->entryCount())
      writePage();
}

void OutputRun::flush()
{
   if (positionInPage != 0)
      writePage();
   end = file->tellp();
   file = nullptr;
}

void OutputRun::prepareForWriting()
{
   if(append)
      file = util::make_unique<std::ofstream>(fileName, std::ios::binary | std::ios::app); else
      file = util::make_unique<std::ofstream>(fileName, std::ios::binary | std::ios::out);
   assert(file->is_open() && file->good());
   start = file->tellp();
   positionInPage = 0;
   validEntries = 0;
}

std::unique_ptr<InputRun> OutputRun::convertToInputRun()
{
   return util::make_unique<InputRun>(start, end-start, fileName);
}

void OutputRun::writePage()
{
   assert(file->is_open());
   assert(file->good());
   file->write(page->begin(), positionInPage * sizeof(uint64_t));
   positionInPage = 0;
   assert(file->is_open());
   assert(file->good());
}

}
