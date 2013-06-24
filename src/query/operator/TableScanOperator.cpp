#include "TableScanOperator.hpp"
#include "harriet/Value.hpp"
#include "query/signature/ColumnSignature.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "schema/RelationSchema.hpp"
#include "segment_manager/Record.hpp"
#include "segment_manager/Record.hpp"
#include "segment_manager/SPSegment.hpp"
#include <iostream>

using namespace std;

namespace dbi {

TableScanOperator::TableScanOperator(const qopt::TableAccessInfo& tableaccessInfo, const set<qopt::ColumnAccessInfo>& requiredColumns, vector<harriet::Value>& globalRegister, uint32_t& registerOffset)
: tableaccessInfo(tableaccessInfo)
, signature(tableaccessInfo, requiredColumns, registerOffset)
, state(kClosed)
, nextPage(tableaccessInfo.segment.endPageId())
, positionInCurrentPage(0)
, globalRegister(globalRegister)
, registerOffset(registerOffset)
{
   registerOffset += signature.getAttributes().size();
}

TableScanOperator::~TableScanOperator()
{
}

const Signature& TableScanOperator::getSignature() const
{
   return signature;
}

void TableScanOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "TableScan " << tableaccessInfo.schema.getName() << " ";
   signature.dump(os);
   os << " into [ " << registerOffset << " ]";
}

void TableScanOperator::open()
{
   assert(state == kClosed);
   state = kOpen;

   // Initialize
   nextPage = tableaccessInfo.segment.beginPageId();
   positionInCurrentPage = 0; // next will load the data
   assert(recordsInCurrentPage.size() == 0);
}

bool TableScanOperator::next()
{
   assert(state == kOpen);

   // Find next page
   while(positionInCurrentPage >= recordsInCurrentPage.size() && nextPage != tableaccessInfo.segment.endPageId()) {
      recordsInCurrentPage = tableaccessInfo.segment.getAllRecordsOfPage(*nextPage);
      positionInCurrentPage = 0;
      ++nextPage;
   }

   // Return if a page was found
   if(positionInCurrentPage < recordsInCurrentPage.size()) {
      auto tuple = tableaccessInfo.schema.recordToTuple(recordsInCurrentPage[positionInCurrentPage].second);
      auto& mapping = signature.getMapping();
      for(uint32_t targetIndex=0; targetIndex<mapping.size(); targetIndex++)
         globalRegister[registerOffset + targetIndex] = move(tuple[mapping[targetIndex]]);
      positionInCurrentPage++;
      return true;
   }
   return false;
}

void TableScanOperator::close()
{
   assert(state == kOpen);
   state = kClosed;
   recordsInCurrentPage.clear();
}

}
