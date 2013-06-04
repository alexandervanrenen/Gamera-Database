#pragma once

#include "Visitor.hpp"
#include <ios>

namespace dbi {

namespace script {

/// 
class CodeGenerationVisitor : public Visitor {
public:
   CodeGenerationVisitor(std::ostream& out);
   virtual ~CodeGenerationVisitor();

   virtual void onPreVisit(RootStatement& root);
   virtual void onPostVisit(RootStatement& root);
   virtual void onPreVisit(SelectStatement& select);
   virtual void onPostVisit(SelectStatement& select);
   virtual void onPreVisit(CreateTableStatement& createTable);
   virtual void onPostVisit(CreateTableStatement& createTable);
   virtual void onPreVisit(InsertStatement& insert);
   virtual void onPostVisit(InsertStatement& insert);
   virtual void onPreVisit(BlockStatement& block);
   virtual void onPostVisit(BlockStatement& block);

private:
   std::ostream& out;
   uint32_t indention;
};

}

}
