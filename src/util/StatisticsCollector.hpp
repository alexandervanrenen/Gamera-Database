#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>
#include <chrono>

class StatisticsCollector {
public:
   void start(const std::string& tag);
   void end(const std::string& tag);
   void log(const std::string& tag, uint32_t ms);

   void print();

private:
   std::unordered_map<std::string, std::pair<uint32_t, uint32_t> > finished;

   std::unordered_map<std::string, std::chrono::time_point<std::chrono::high_resolution_clock>> running;
};
