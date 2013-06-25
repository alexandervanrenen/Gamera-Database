#include "Projection.hpp"
#include "harriet/Expression.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "query/util/ColumnAccessInfo.hpp"

using namespace std;

namespace dbi {

namespace qopt {

Projection::Projection(std::unique_ptr<harriet::Expression> expression)
: expression(move(expression))
, resultRegisterSlot(111111)
{
   ostringstream os;
   this->expression->print(os);
   prettyExpression = os.str();
}

Projection::~Projection()
{
}

set<uint32_t> Projection::getRequiredTables() const
{
   set<uint32_t> result;
   for(auto& iter : requiredColumns)
      result.insert(iter.tableIndex);
   return result;
}

void Projection::dump(ostream& os) const
{
   os << "expression: " << prettyExpression << " --> " << alias << " --> " << resultRegisterSlot;
   os << endl << "columns:";
   for(auto iter : requiredColumns)
      os << " " << iter.columnReference.str() << "(" << iter.tableIndex << "." << iter.columnIndex << ")";
   os << endl << "tables:";
   for(auto iter : getRequiredTables())
      os << " " << iter;
   os << endl;
}

}

}
