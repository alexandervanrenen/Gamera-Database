#pragma once

#include "harriet/ScriptLanguage.hpp"
#include <ios>

namespace dbi {

/// Each plan has a root. The root is not able to produce tuples, but takes care of doing something with the tuples produced by the child (e.g. the insert-root would insert the tuples into a table)
class RootOperator {
public:
   virtual ~RootOperator();
   virtual void dump(std::ostream& os) const = 0;

   virtual void checkTypes() const throw(harriet::Exception) =  0;

   virtual void execute() = 0;
};

}
