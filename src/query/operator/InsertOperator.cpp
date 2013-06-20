#include "harriet/Value.hpp"
#include "InsertOperator.hpp"
#include "Operator.hpp"
#include "schema/RelationSchema.hpp"
#include "segment_manager/SPSegment.hpp"
#include "query/signature/ColumnSignature.hpp"
#include "query/signature/Signature.hpp"
#include <iostream>

using namespace std;

namespace dbi {

InsertOperator::InsertOperator(unique_ptr<Operator> source, SPSegment& target, const RelationSchema& targetSchema, vector<harriet::Value>& globalRegister)
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
   auto& sourceSchema = source->getSignature();
   if(sourceSchema.getAttributes().size() != targetSchema.getAttributes().size())
      throw harriet::Exception{"Insert " + targetSchema.getName() + ": expected " + to_string(targetSchema.getAttributes().size()) + " arguments, " + to_string(sourceSchema.getAttributes().size()) + " provided."};
   for(uint32_t i=0; i<sourceSchema.getAttributes().size(); i++)
      if(!harriet::isImplicitCastPossible(sourceSchema.getAttributes()[i].type, targetSchema.getAttributes()[i].type))
         throw harriet::Exception{"Insert into " + targetSchema.getName() + ": invalid conversion from '" + sourceSchema.getAttributes()[i].type.str() + "' to '" + targetSchema.getAttributes()[i].type.str() + "' for argument " + to_string(i) + "."};
}

void InsertOperator::execute()
{
   source->open();
   while(source->next()) {
      // Materialize in result, as the global register is not ordered OOO
      vector<harriet::Value> result;
      for(uint32_t i=0; i<source->getSignature().getAttributes().size(); i++)
         result.push_back(move(globalRegister[i]));
      target.insert(targetSchema.tupleToRecord(result));
   }
   source->close();
}

}
