#ifndef __FileNameProvider
#define __FileNameProvider

#include <array>
#include <string>

class FileNameProvider {
public:
   FileNameProvider(const std::string& nameBase)
   {
      for(auto& iter : names)
         iter = nameBase + iter;
   }

   const std::string& getNext()
   {
      id = (id + 1) % names.size();
      return names[id];
   }

   void removeAll()
   {
      for(auto& iter : names)
         remove(iter.c_str());
   }

private:
   uint8_t id = 0;
   std::array<std::string, 2> names { {"yin", "yang"}};
};

#endif
