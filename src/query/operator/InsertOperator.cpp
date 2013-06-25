#include "InsertOperator.hpp"
#include "ProjectionOperator.hpp"
#include "harriet/Value.hpp"
#include "Operator.hpp"
#include "schema/RelationSchema.hpp"
#include "query/util/ColumnAccessInfo.hpp"
#include "query/util/GlobalRegister.hpp"
#include "segment_manager/SPSegment.hpp"
#include <iostream>

using namespace std;

namespace dbi {

InsertOperator::InsertOperator(unique_ptr<ProjectionOperator> source, SPSegment& target, const RelationSchema& targetSchema, qopt::GlobalRegister& globalRegister)
: source(move(source))
, target(target)
, targetSchema(targetSchema)
, globalRegister(globalRegister)
{
}

InsertOperator::~InsertOperator()
{
}

void InsertOperator::dump(ostream& os) const
{
   os << "Insert " << targetSchema.getName() << endl;
   source->dump(os, 3);
}

void InsertOperator::checkTypes() const throw(harriet::Exception)
{
   // See if we can insert the provided types into the table
   auto& columns = source->getSuppliedColumns();
   if(columns.size() != targetSchema.getAttributes().size())
      throw harriet::Exception{"Insert " + targetSchema.getName() + ": expected " + to_string(targetSchema.getAttributes().size()) + " arguments, " + to_string(columns.size()) + " provided."};
   for(uint32_t i=0; i<columns.size(); i++)
      if(!harriet::isImplicitCastPossible(columns[i].columnSchema.type, targetSchema.getAttributes()[i].type))
         throw harriet::Exception{"Insert into " + targetSchema.getName() + ": invalid conversion from '" + columns[i].columnSchema.type.str() + "' to '" + targetSchema.getAttributes()[i].type.str() + "' for argument " + to_string(i) + "."};
}

void InsertOperator::execute()
{
   std::vector<uint32_t> globalRegisterIndexes;
   for(auto iter : source->getSuppliedColumns())
      globalRegisterIndexes.push_back(globalRegister.getColumnIndex(iter.tableIndex, iter.columnSchema.name));

   source->open();
   while(source->next()) {
      // Materialize in result, as the global register is not ordered
      vector<harriet::Value> result;
      for(auto sourceIndex : globalRegisterIndexes)
         result.emplace_back(move(globalRegister.getValue(sourceIndex)));
      target.insert(targetSchema.tupleToRecord(result));
   }
   source->close();
}

}
