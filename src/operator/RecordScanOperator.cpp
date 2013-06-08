#include "segment_manager/Record.hpp"
#include "segment_manager/SPSegment.hpp"
#include "schema/RelationSchema.hpp"
#include "RecordScanOperator.hpp"
#include <iostream>

using namespace std;

namespace dbi {

RecordScanOperator::RecordScanOperator(SPSegment& input)
: segment(input)
, nextPage(input.endPageId())
, positionInCurrentPage(0)
, state(kClosed)
{
}

RecordScanOperator::~RecordScanOperator()
{
}

const RelationSchema& RecordScanOperator::getSignature() const
{
   throw;
}

void RecordScanOperator::checkTypes() const throw(harriet::Exception)
{
   return;
}

void RecordScanOperator::open()
{
   assert(state == kClosed);
   state = kOpen;

   // Initialize
   nextPage = segment.beginPageId();
   positionInCurrentPage = 0; // next will load the data
   assert(recordsInCurrentPage.size() == 0);
}

bool RecordScanOperator::next()
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

vector<unique_ptr<harriet::Value>> RecordScanOperator::getOutput()
{
   throw;
}

const pair<TupleId, Record>& RecordScanOperator::getRecord()
{
   assert(positionInCurrentPage < recordsInCurrentPage.size());
   return recordsInCurrentPage[positionInCurrentPage];
}

void RecordScanOperator::close()
{
   assert(state == kOpen);
   state = kClosed;
}

}
