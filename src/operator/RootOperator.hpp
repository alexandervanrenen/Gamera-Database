#pragma once

#include "harriet/ScriptLanguage.hpp"
#include <array>
#include <cstdint>
#include <memory>

namespace dbi {

/// 
class RootOperator {
public:
   virtual ~RootOperator();
   virtual void dump(std::ostream& os) const = 0;

   virtual void checkTypes() const throw(harriet::Exception) = 0;

   virtual void execute() = 0;
};

}
