#ifndef __MinHeap
#define __MinHeap

#include "InputRun.hpp"
#include <cassert>
#include <memory>
#include <set>
#include <vector>

namespace dbi {

/// Fast RunHeap -- keeps track of the run with minimal next value
class RunHeap {
public:
   RunHeap()
   : data([](const InputRun* l, const InputRun* r) {return l->peekNext() < r->peekNext();})
   {
   }

   void push(std::unique_ptr<InputRun> run)
   {
      // Just add
      data.insert(run.get());
      dataMem.push_back(move(run));
   }

   uint64_t getMin()
   {
      // Locate
      assert(hasMore());
      auto nextRunIter = data.begin();
      InputRun* nextRun = *nextRunIter;
      uint64_t result = (*nextRunIter)->getNext();
      data.erase(nextRunIter);

      if(nextRun->hasNext())
         data.insert(nextRun);

      return result;
   }

   bool hasMore()
   {
      return !data.empty();
   }

   uint32_t size()
   {
      return data.size();
   }

private:
   std::multiset<InputRun*, std::function<bool(InputRun*, InputRun*)>> data;
   std::vector<std::unique_ptr<InputRun>> dataMem;
};

}

#endif
