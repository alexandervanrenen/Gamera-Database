#pragma once

#include <memory>
#include <string>

namespace dbi {

class Transaction;

namespace util {
   class DynamicLinker;
}

namespace script {

class Script {

   Script(const std::string& name, const std::string& code);
   ~Script();

   void run(Transaction& transaction);

private:
   std::unique_ptr<util::DynamicLinker> code;
};
  
}

}
