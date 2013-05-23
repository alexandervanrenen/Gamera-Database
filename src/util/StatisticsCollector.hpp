#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>

namespace dbi {

namespace util {

template<bool debug>
class StatisticsCollector {
public:
   void start(const std::string& tag);
   void end(const std::string& tag);
   void count(const std::string& tag, int32_t val);

   void print(std::ostream& stream);
};

template<>
class StatisticsCollector<true> {
public:
   StatisticsCollector(const std::string& name)
   : name(name)
   {
   }

   void count(const std::string& tag, int32_t val)
   {
      std::unique_lock<std::mutex> l(guard);
      counters[tag] += val;
   }

   void start(const std::string& tag)
   {
      std::unique_lock<std::mutex> l(guard);
      running[tag] = std::chrono::high_resolution_clock::now();
   }

   void end(const std::string& tag)
   {
      std::unique_lock<std::mutex> l(guard);
      auto start = running[tag];
      auto end = std::chrono::high_resolution_clock::now();
      finished[tag].first += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
      finished[tag].second++;
   }

   void print(std::ostream& stream) const
   {
      stream << name << std::endl;
      stream << std::string(name.size(), '=') << std::endl;
      for(auto iter : finished)
         stream << iter.first << " : " << iter.second.first / 1000.0f << " ms [" << iter.second.second << ": " << iter.second.first / 1000.0f / iter.second.second << " ms ]" << std::endl;
      for(auto iter : counters)
         stream << iter.first << " : " << iter.second << std::endl;
   }

private:
   std::unordered_map<std::string, std::pair<uint32_t, uint32_t> > finished;
   std::unordered_map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> running;
   std::unordered_map<std::string, uint32_t> counters;
   std::string name;
   std::mutex guard;
};

template<>
class StatisticsCollector<false> {
public:
   StatisticsCollector(const std::string&)
   {
   }

   void start(const std::string&)
   {
   }

   void end(const std::string&)
   {
   }

   void count(const std::string&, int32_t)
   {
   }

   void print(std::ostream&) const
   {
   }
};

}

}
