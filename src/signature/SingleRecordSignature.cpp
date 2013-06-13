#include "SingleRecordSignature.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"

using namespace std;

namespace dbi {

SingleRecordSignature::SingleRecordSignature(const vector<harriet::Value>& values)
{
   for(auto& iter : values)
      attributes.push_back(AttributeSignature{"", "", true, true, iter.type});
}

}
