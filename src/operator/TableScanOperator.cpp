#include "segment_manager/Record.hpp"
#include "schema/RelationSchema.hpp"
#include "TableScanOperator.hpp"
#include "RecordScanOperator.hpp"
#include <iostream>

using namespace std;

namespace dbi {

TableScanOperator::TableScanOperator(std::unique_ptr<RecordScanOperator> scanner, const RelationSchema& schema)
: scanner(move(scanner))
, state(kClosed)
, schema(schema)
{
}

TableScanOperator::~TableScanOperator()
{
}

const RelationSchema& TableScanOperator::getSignatur() const
{
   return schema;
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
   cout << "almost there" << endl;
   throw;
}

void TableScanOperator::close()
{
   assert(state == kOpen);
   scanner->close();
   state = kClosed;
}

}
