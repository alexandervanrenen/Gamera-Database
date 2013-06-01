#include "segment_manager/Record.hpp"
#include "segment_manager/SPSegment.hpp"
#include "TableScanOperator.hpp"
#include <iostream>

using namespace std;

namespace dbi {

TableScanOperator::TableScanOperator(SPSegment& input)
: segment(input)
, nextPage(input.endPageId())
, positionInCurrentPage(0)
, state(kClosed)
{
}

void TableScanOperator::open()
{
   assert(state == kClosed);
   state = kOpen;

   // Initialize
   nextPage = segment.beginPageId();
   positionInCurrentPage = 0; // next will load the data
   assert(recordsInCurrentPage.size() == 0);
}

bool TableScanOperator::next()
{
   assert(state == kOpen);

   // Check if end is reached
   if(positionInCurrentPage == recordsInCurrentPage.size() && nextPage == segment.endPageId())
      return false;

   // Current page has more elements
   positionInCurrentPage++;
   if(positionInCurrentPage < recordsInCurrentPage.size()) {
      return true;
   }

   // Find next page
   while(positionInCurrentPage >= recordsInCurrentPage.size() && nextPage != segment.endPageId()) {
      recordsInCurrentPage = segment.getAllRecordsOfPage(*nextPage);
      positionInCurrentPage = 0;
      ++nextPage;
   }

   // Return if a page was found
   return positionInCurrentPage < recordsInCurrentPage.size();
}

const pair<TupleId, Record>& TableScanOperator::getOutput()
{
   assert(positionInCurrentPage < recordsInCurrentPage.size());
   return recordsInCurrentPage[positionInCurrentPage];
}

void TableScanOperator::close()
{
   assert(state == kOpen);
   state = kClosed;
}

TableScanOperator::~TableScanOperator()
{
}

}
