#include "SelectionSignature.hpp"
#include "schema/Common.hpp"

using namespace std;

namespace dbi {

SelectionSignature::SelectionSignature(const Signature& source, const Predicate& predicate)
{
   predicates.push_back(predicate);
}

}
