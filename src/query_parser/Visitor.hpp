#pragma once

#include <ios>

namespace dbi {

namespace script {

class SelectStatement;
class CreateTableStatement;
class BlockStatement;

/// 
class Visitor {
public:
   virtual ~Visitor();

   virtual void onPreVisit(SelectStatement& select) = 0;
   virtual void onPostVisit(SelectStatement& select) = 0;
   virtual void onPreVisit(CreateTableStatement& createTable) = 0;
   virtual void onPostVisit(CreateTableStatement& createTable) = 0;
   virtual void onPreVisit(BlockStatement& block) = 0;
   virtual void onPostVisit(BlockStatement& block) = 0;
};

/// 
class PrintVisitor : public Visitor {
public:
   PrintVisitor(std::ostream& out);
   virtual ~PrintVisitor();

   virtual void onPreVisit(SelectStatement& select);
   virtual void onPostVisit(SelectStatement& select);
   virtual void onPreVisit(CreateTableStatement& createTable);
   virtual void onPostVisit(CreateTableStatement& createTable);
   virtual void onPreVisit(BlockStatement& block);
   virtual void onPostVisit(BlockStatement& block);
private:
   std::ostream& out;
};

}

}
