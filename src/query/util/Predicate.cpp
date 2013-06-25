#include "Predicate.hpp"
#include "harriet/Expression.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include <iostream>

using namespace std;

namespace dbi {

namespace qopt {

Predicate::Predicate(unique_ptr<harriet::Expression> condition)
: condition(move(condition))
{
   ostringstream os;
   this->condition->print(os);
   prettyCondition = os.str();
}

Predicate::~Predicate()
{
}

set<uint32_t> Predicate::getRequiredTables() const
{
   set<uint32_t> result;
   for(auto& iter : requiredColumns)
      result.insert(iter.tableIndex);
   return result;
}

void Predicate::dump(ostream& os) const
{
   os << "condition: ";
   condition->print(os);
   os << endl << "columns:";
   for(auto iter : requiredColumns)
      iter.columnReference.str();
   os << endl << "tables:";
   os << endl;
}

}

}
   
