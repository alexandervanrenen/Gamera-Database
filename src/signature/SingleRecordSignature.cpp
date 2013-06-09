#include "SingleRecordSignature.hpp"
#include "harriet/Expression.hpp"

using namespace std;

namespace dbi {

SingleRecordSignature::SingleRecordSignature(const vector<unique_ptr<harriet::Value>>& values)
{
   for(auto& iter : values)
      attributes.push_back(AttributeSignature{"", "", true, true, iter->getResultType()});
}

}
