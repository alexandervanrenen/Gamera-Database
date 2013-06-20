#include "CrossProductSignature.hpp"
#include "query/signature/ColumnSignature.hpp"
#include <algorithm>

using namespace std;

namespace dbi {

CrossProductSignature::CrossProductSignature(const Signature& lhs, const Signature& rhs)
{
   attributes.resize(lhs.getAttributes().size() + rhs.getAttributes().size());
   copy(lhs.getAttributes().begin(), lhs.getAttributes().end(), attributes.begin());
   copy(rhs.getAttributes().begin(), rhs.getAttributes().end(), attributes.begin() + lhs.getAttributes().size());
}

}
