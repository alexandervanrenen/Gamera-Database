#include "InsertOperator.hpp"
#include "Operator.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Value.hpp"
#include "segment_manager/SPSegment.hpp"
#include "signature/Signature.hpp"
#include <iostream>

using namespace std;

namespace dbi {

InsertOperator::InsertOperator(unique_ptr<Operator> source, SPSegment& target, const RelationSchema& targetSchema)
: source(move(source))
, target(target)
, targetSchema(targetSchema)
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
   // Check if everything below works out
   source->checkTypes();

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
      auto result = source->getOutput();
      target.insert(targetSchema.tupleToRecord(result));
   }
   source->close();
}

}
