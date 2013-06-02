#include "DynamicLinker.hpp"
#include <fstream>
#include <dlfcn.h>

using namespace std;

namespace dbi {

namespace util {

DynamicLinker::DynamicLinker()
{
}

DynamicLinker::~DynamicLinker()
{
   dlclose(libhandle);
}

void DynamicLinker::compile(const string& targetFile, const string& flags)
{
   // Compile code
   string cmd1;
   FILE* compile = popen(("g++ -c " + flags + " -std=c++11 -Wall -fPIC " + targetFile + ".cpp -o " + targetFile + ".o").c_str(), "r");
   if(compile==nullptr || pclose(compile)<0)
      throw;

   // Build shared object
   FILE* shared = popen(("g++ " + flags + " -std=c++11 -shared -o " + targetFile + ".so " + targetFile + ".o").c_str(), "r");
   if(shared==nullptr || pclose(shared)<0)
      throw;

   // Load shared object
   libhandle = dlopen((targetFile+".so").c_str(),RTLD_LAZY);
   if(libhandle == nullptr)
      throw;
}

void* DynamicLinker::extractFunction(const string& name) const
{
   void* func = dlsym(libhandle, name.c_str());
   if(!func)
      throw;
   return func;
}

}

}
