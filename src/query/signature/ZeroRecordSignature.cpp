#include "ZeroRecordSignature.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "ColumnSignature.hpp"
#include "harriet/VariableType.hpp"

using namespace std;

namespace dbi {

ZeroRecordSignature::ZeroRecordSignature(std::set<qopt::ColumnAccessInfo>& projections)
{
   for(auto& iter : projections)
      attributes.push_back(ColumnSignature{iter.columnSchema.name, "", true, harriet::VariableType::createUndefinedType(), 0, iter.tableIndex}); // fishy ..
}

}
