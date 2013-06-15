#include "ProjectionSignature.hpp"
#include "query_parser/Common.hpp" // AAA

using namespace std;

namespace dbi {

ProjectionSignature::ProjectionSignature(const Signature& source, const vector<ColumnReference>& target)
{
   // Create projection
   for(auto& iter : target)
      projection.push_back(source.getAttributeIndex(iter.tableQualifier, iter.columnName));

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
