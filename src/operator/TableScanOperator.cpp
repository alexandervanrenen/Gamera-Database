#include "segment_manager/Record.hpp"
#include "schema/RelationSchema.hpp"
#include "TableScanOperator.hpp"
#include "harriet/Expression.hpp"
#include "segment_manager/Record.hpp"
#include "segment_manager/SPSegment.hpp"
#include "schema/RelationSchema.hpp"
#include "harriet/Value.hpp"
#include <iostream>

using namespace std;

namespace dbi {

TableScanOperator::TableScanOperator(SPSegment& source, const RelationSchema& underlyingSchema, const string& alias)
: source(source)
, underlyingSchema(underlyingSchema)
, signature(underlyingSchema, alias)
, state(kClosed)
, nextPage(source.endPageId())
, positionInCurrentPage(0)
{
}

TableScanOperator::~TableScanOperator()
{
}

const Signature& TableScanOperator::getSignature() const
{
   return signature;
}

void TableScanOperator::checkTypes() const throw(harriet::Exception)
{
   return;
}

void TableScanOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "TableScan " << underlyingSchema.getName() << " ";
   signature.dump(os);
}

void TableScanOperator::open()
{
   assert(state == kClosed);
   state = kOpen;

   // Initialize
   nextPage = source.beginPageId();
   positionInCurrentPage = 0; // next will load the data
   assert(recordsInCurrentPage.size() == 0);
}

bool TableScanOperator::next()
{
   assert(state == kOpen);

   // Check if end is reached
   if(positionInCurrentPage == recordsInCurrentPage.size() && nextPage == source.endPageId())
      return false;

   // Current page has more elements
   positionInCurrentPage++;
   if(positionInCurrentPage < recordsInCurrentPage.size()) {
      return true;
   }

   // Find next page
   while(positionInCurrentPage >= recordsInCurrentPage.size() && nextPage != source.endPageId()) {
      recordsInCurrentPage = source.getAllRecordsOfPage(*nextPage);
      positionInCurrentPage = 0;
      ++nextPage;
   }

   // Return if a page was found
   return positionInCurrentPage < recordsInCurrentPage.size();
}

vector<unique_ptr<harriet::Value>> TableScanOperator::getOutput()
{
   assert(positionInCurrentPage < recordsInCurrentPage.size());
   auto tuple = underlyingSchema.recordToTuple(recordsInCurrentPage[positionInCurrentPage].second);
   return move(tuple);
}

void TableScanOperator::close()
{
   assert(state == kOpen);
   state = kClosed;
   recordsInCurrentPage.clear();
}

}
