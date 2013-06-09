#include "ProjectionSignature.hpp"
#include "schema/Common.hpp"

using namespace std;

namespace dbi {

ProjectionSignature::ProjectionSignature(const Signature& source, const vector<ColumnIdentifier>& target)
{
   // Create projection
   for(auto& iter : target)
      projection.push_back(source.getAttributeIndex(iter.tableIdentifier, iter.columnIdentifier));

   // Create new signature from projection
   for(uint32_t i=0; i<projection.size(); i++) {
      attributes.push_back(source.getAttributes()[projection[i]]);
      attributes.back().name = source.getAttributes()[projection[i]].name;
      attributes.back().alias = source.getAttributes()[projection[i]].alias;
   }
}

vector<uint32_t> ProjectionSignature::getProjection() const
{
   return projection;
}

}
