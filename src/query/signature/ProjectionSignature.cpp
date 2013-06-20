#include "ProjectionSignature.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "query/signature/ColumnSignature.hpp"
#include <iostream>

using namespace std;

namespace dbi {

ProjectionSignature::ProjectionSignature(const vector<qopt::ColumnAccessInfo>& target)
: target(target)
{
}

ProjectionSignature::~ProjectionSignature()
{
}

void ProjectionSignature::prepare(const Signature& source)
{
   // Create projection
   for(auto& iter : target)
      attributes.push_back(source.getAttribute(iter.tableIndex, iter.columnSchema.name));
}

set<qopt::ColumnAccessInfo> ProjectionSignature::getRequiredColumns() const
{
   set<qopt::ColumnAccessInfo> result;
   for(auto& iter : target)
      result.insert(iter);
   return result;
}

}
