#pragma once

#include "util/Math.hpp"
#include <cassert>
#include <vector>
#include <iostream>

template<class T, T max>
class LoserTree {
public:
   LoserTree(const std::vector<T>& data);
   void set(const uint32_t index, const T& t);

   const T popAndReplace(const T& t);
   const T min();

private:
   std::vector<uint32_t> tree;
   std::vector<T> data;
};

template<class T, T max>
LoserTree<T, max>::LoserTree(const std::vector<T>& data)
: data(data)
{
   // Check
   assert(data.size()!=0 && "do not abuse the tree .. :(");
   assert(data.size()==util::nextPowerOfTwo(data.size()) && "need power of to elements ..");

   // Allocate -- Need winner for building
   tree.resize(util::nextPowerOfTwo(data.size()), 0);
   std::vector<uint32_t> winner(util::nextPowerOfTwo(data.size()), 0);

   // Initial build -- First level
   for(uint32_t i=winner.size()-1; i>=winner.size()/2; i--) {
      uint32_t left = 2*i - data.size();
      uint32_t right = 2*i - data.size() + 1;
      if(data[left] < data[right]) {
         winner[i] = left;
         tree[i] = right;
      } else {
         winner[i] = right;
         tree[i] = left;
      }
   }

   // Initial build -- Internal nodes
   for(uint32_t i=winner.size()/2-1; i>=1; i--) {
      uint32_t left = i*2;
      uint32_t right = i*2 + 1;
      if(data[winner[left]] < data[winner[right]]) {
         winner[i] = winner[left];
         tree[i] = winner[right];
      } else {
         winner[i] = winner[right];
         tree[i] = winner[left];
      }
   }

   // Initial build -- Root
   tree[0] = winner[1];
}

template<class T, T max>
const T LoserTree<T, max>::popAndReplace(const T& replacement)
{
   // Get result
   uint32_t position = tree[0];
   T result = data[position];
   data[position] = replacement;

   uint32_t winnerPosition = position;
   position = position/2 + data.size()/2;

   // Update internal nodes
   while(position >= 1) {
      if(data[tree[position]] < data[winnerPosition])
         std::swap(winnerPosition, tree[position]);
      position /= 2;
   }

   // Update root
   tree[0] = winnerPosition;
   return result;
}

template<class T, T max>
const T LoserTree<T, max>::min()
{
   return data[tree[0]];
}
