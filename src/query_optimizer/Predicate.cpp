#include "Predicate.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

namespace qopt {

void Predicate::dump(ostream& os) const
{
   os << "condition: ";
   condition->print(os);
   os << endl << "columns:";
   for(auto iter : columns)
      os << " " << iter.scriptName;
   os << endl << "tables:";
   for(auto iter : tables)
      os << " " << iter;
   os << endl;
}

}

}
   
