#include "segment_manager/Record.hpp"
#include "schema/RelationSchema.hpp"
#include "TableScanOperator.hpp"
#include "RecordScanOperator.hpp"
#include "harriet/Expression.hpp"
#include <iostream>

using namespace std;

namespace dbi {

TableScanOperator::TableScanOperator(std::unique_ptr<RecordScanOperator> scanner, const RelationSchema& schema, const string& alias)
: scanner(move(scanner))
, state(kClosed)
, schema(schema)
, signature(schema, alias)
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
   os << "|" << string(lvl, '.') << "TableScan " << schema.getName() << endl;
}

void TableScanOperator::open()
{
   assert(state == kClosed);
   scanner->open();
   state = kOpen;
}

bool TableScanOperator::next()
{
   assert(state == kOpen);
   return scanner->next();
}

vector<unique_ptr<harriet::Value>> TableScanOperator::getOutput()
{
   auto& record = scanner->getRecord();
   return schema.recordToTuple(record.second);
}

void TableScanOperator::close()
{
   assert(state == kOpen);
   scanner->close();
   state = kClosed;
}

}
