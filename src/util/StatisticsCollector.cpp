#include "StatisticsCollector.hpp"
#include <iostream>

using namespace std;

void StatisticsCollector::log(const string& tag, uint32_t ms)
{
   finished[tag].first += ms;
   finished[tag].second++;
}

void StatisticsCollector::start(const string& tag)
{
   running[tag] = chrono::high_resolution_clock::now();
}

void StatisticsCollector::end(const string& tag)
{
   auto start = running[tag];
   auto end = chrono::high_resolution_clock::now();
   finished[tag].first += chrono::duration_cast<chrono::milliseconds>(end - start).count();
   finished[tag].second++;
}

void StatisticsCollector::print()
{
   for(auto iter : finished)
      cout << iter.first << " : " << iter.second.first / 1000.0f << " ms [" << iter.second.second << ": " << iter.second.first / 1000.0f / iter.second.second << " ms ]" << endl;
}
