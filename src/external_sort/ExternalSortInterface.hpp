#pragma once

#include "ExternalSort.hpp"
#include "util/Math.hpp"
#include <iostream>
#include <unistd.h>
#include <string>

std::string getFileName(int fd) {
   char buf[4096];
   ssize_t len = readlink(("/proc/self/fd/" + std::to_string(fd)).c_str(), buf, sizeof(buf)-1);
   if (len == -1)
      throw;
   buf[len] = '\0';
   return std::string(buf);
}

void externalSort(int fdInput, uint64_t, int fdOutput, uint64_t memory)
{
   // Convert fds back to strings :(
   std::string input = getFileName(fdInput);
   std::string output = getFileName(fdOutput);

   // Try to correct input (to small values or not power of tow)
   // The sort algorithm is not trained to handle stupid values :p
   if(memory != util::nextPowerOfTwo(memory))
      memory = util::nextPowerOfTwo(memory) / 2;
   uint64_t pageSize = 4096;
   while(pageSize!=0 && (memory%pageSize != 0 || pageSize*3 > memory)) 
      pageSize /= 2;
   if(pageSize == 0) {
      std::cout << "sorry using to little memory .. " << std::endl;
      throw;
   }

   // Run sort
   dbi::ExternalSort sorty(input, output, pageSize, memory, false);
   sorty.run();
}
