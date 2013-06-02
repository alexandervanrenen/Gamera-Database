#pragma once

#include <string>
#include <functional>

namespace dbi {

namespace util {

class DynamicLinker {
public:
   DynamicLinker();
   ~DynamicLinker();
   void compile(const std::string& name, const std::string& flags);

   template <typename Signature>
   std::function<Signature> getFunction(const std::string& name) { return reinterpret_cast<Signature*>(extractFunction(name)); }

private:
   void* extractFunction(const std::string& name) const;

   void* libhandle;
};

}

}