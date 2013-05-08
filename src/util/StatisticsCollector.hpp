#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>
#include <chrono>
#include <sstream>

template<bool debug>
class StatisticsCollector {
public:
   void start(const std::string& tag);
   void end(const std::string& tag);
   void log(const std::string& tag, uint32_t ms);

   void print(std::ostream& stream);
};

template<>
class StatisticsCollector<true> {
public:
   void log(const std::string& tag, uint32_t ms)
   {
      finished[tag].first += ms;
      finished[tag].second++;
   }

   void start(const std::string& tag)
   {
      running[tag] = std::chrono::high_resolution_clock::now();
   }

   void end(const std::string& tag)
   {
      auto start = running[tag];
      auto end = std::chrono::high_resolution_clock::now();
      finished[tag].first += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      finished[tag].second++;
   }

   void print(std::ostream& stream)
   {
      for(auto iter : finished)
         stream << iter.first << " : " << iter.second.first / 1000.0f << " ms [" << iter.second.second << ": " << iter.second.first / 1000.0f / iter.second.second << " ms ]" << std::endl;
   }

private:
   std::unordered_map<std::string, std::pair<uint32_t, uint32_t> > finished;
   std::unordered_map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> running;
};

template<>
class StatisticsCollector<false> {
public:
   void log(const std::string&, uint32_t)
   {
   }

   void start(const std::string&)
   {
   }

   void end(const std::string&)
   {
   }
};
