#include "SortOperator.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "query/util/Projection.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include "query/util/GlobalRegister.hpp"
#include "query/analyser/ExpressionOptimizer.hpp"
#include "util/Utility.hpp"
#include "btree/IndexKey.hpp"
#include "btree/IndexKeyComparator.hpp"
#include "external_sort/ExternalSort.hpp"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace std;

namespace dbi {

SortOperator::SortOperator(unique_ptr<Operator> source, const std::vector<std::string>& orderBy, qopt::GlobalRegister& globalRegister)
: source(move(source))
, state(kClosed)
, orderBy(orderBy)
, globalRegister(globalRegister)
, alreadySorted(false)
{
   state = kClosed;

   // Build a vector containing pointers into the global register in the required sort order
   vector<harriet::Value*> registerPointers;
   vector<harriet::VariableType> types;
   vector<uint32_t> remainingColumns(globalRegister.size());
   iota(remainingColumns.begin(), remainingColumns.end(), 0);
   for(auto& column : orderBy) {
      uint32_t index = globalRegister.getColumnIndex(column);
      remainingColumns.erase(remove(remainingColumns.begin(), remainingColumns.end(), index), remainingColumns.end());
      registerPointers.push_back(&globalRegister.getSlotValue(index));
      types.push_back(globalRegister.getSlotValue(index).type);
   }
   for(auto iter : remainingColumns) {
      registerPointers.push_back(&globalRegister.getSlotValue(iter));
      types.push_back(globalRegister.getSlotValue(iter).type);
   }
   key = util::make_unique<IndexKey>(registerPointers);
   sorty = util::make_unique<ExternalSort>(IndexKeySchema(types), IndexKeyComparator(IndexKeySchema(types)));
}

SortOperator::~SortOperator()
{
}

void SortOperator::dump(ostream& os, uint32_t lvl) const
{
   os << "|" << string(lvl, '.') << "Sort ";
   for(auto& iter : orderBy)
      os << iter;
   os << " order(";
   key->dump(os, &globalRegister.getSlotValue(0));
   os << ")" << endl;
   source->dump(os, lvl+3);
}

void SortOperator::open()
{
   assert(state == kClosed);
   source->open();
   if(alreadySorted)
      sorty->rewind();
   state = kOpen;
}

bool SortOperator::next()
{
   assert(state == kOpen);

   // Do the sort only once
   if(!alreadySorted) {
      while(source->next())
         sorty->addTuple(*key);
      sorty->sort();
   }
   alreadySorted = true;

   // Read values back from file
   if(!sorty->hasNextTuple())
      return false;
   sorty->readNextTuple(*key);
   return true;
}

void SortOperator::close()
{
   assert(state == kOpen);
   source->close();
   state = kClosed;
}

}
