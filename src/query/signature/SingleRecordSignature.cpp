#include "SingleRecordSignature.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include "query/signature/ColumnSignature.hpp"

using namespace std;

namespace dbi {

SingleRecordSignature::SingleRecordSignature(const vector<harriet::Value>& values)
{
   for(auto& iter : values)
      attributes.push_back(ColumnSignature{"", "", true, iter.type, -1, 0}); // fishy ..
}

}
