#include "TableScanOperator.hpp"
#include "harriet/Value.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "schema/RelationSchema.hpp"
#include "segment_manager/Record.hpp"
#include "segment_manager/Record.hpp"
#include "segment_manager/SPSegment.hpp"
#include <iostream>

using namespace std;

namespace dbi {

TableScanOperator::TableScanOperator(const qopt::TableAccessInfo& tableAccessInfo, qopt::GlobalRegister& globalRegister)
: tableaccessInfo(tableAccessInfo)
, signature(tableAccessInfo, globalRegister)
, state(kClosed)
, nextPage(tableAccessInfo.segment.endPageId())
, positionInCurrentPage(0)
{
}

TableScanOperator::~TableScanOperator()
{
}

void TableScanOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "TableScan " << tableaccessInfo.schema.getName() << " ";
   signature.dump(os);
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
      auto& record = recordsInCurrentPage[positionInCurrentPage].second;
      signature.loadRecordIntoGlobalRegister(record);
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
