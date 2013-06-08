#include "InsertOperator.hpp"
#include "Operator.hpp"
#include "harriet/Expression.hpp"
#include "segment_manager/SPSegment.hpp"
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

void InsertOperator::checkTypes() const throw(harriet::Exception)
{
   // Check if everything below works out
   source->checkTypes();

   // See if we can insert the provided types into the table
   auto& sourceSchema = source->getSignature();
   if(sourceSchema.getAttributes().size() != targetSchema.getAttributes().size())
      throw harriet::Exception{"Insert: expected " + to_string(targetSchema.getAttributes().size()) + " arguments, " + to_string(sourceSchema.getAttributes().size()) + " provided."};
   for(uint32_t i=0; i<sourceSchema.getAttributes().size(); i++)
      if(!harriet::isImplicitCastPossible(sourceSchema.getAttributes()[i].type, targetSchema.getAttributes()[i].type))
         throw harriet::Exception{"Insert: invalid conversion from '" + harriet::typeToName(sourceSchema.getAttributes()[i].type) + "' to '" + harriet::typeToName(targetSchema.getAttributes()[i].type) + "'."};
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
