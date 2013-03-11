#ifndef __MinHeap
#define __MinHeap

#include "Run.hpp"
#include <vector>
#include <memory>
#include <cassert>

/// Fast RunHeap -- keeps track of the run with minimal next value
template<class T>
class RunHeap {
public:
	RunHeap()
	{
	}

	void push(std::unique_ptr<Run<T>> run)
	{
		// Just add
		data.push_back(move(run));
	}

	T getMin()
	{
		// Locate
		assert(hasMore());
		uint32_t minIndex = 0;
		for (uint32_t i = 1; i < data.size(); ++i)
			if(data[i]->peekNext() < data[minIndex]->peekNext())
				minIndex = i;

      // Remove
      T value = data[minIndex]->getNext();
      if (!data[minIndex]->hasNext())
         data.erase(data.begin() + minIndex);

      return value;
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
	std::vector<std::unique_ptr<Run<T>>> data;
};

#endif
