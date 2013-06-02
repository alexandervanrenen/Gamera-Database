#pragma once

namespace dbi {

namespace script {

class RootStatement;
class SelectStatement;
class CreateTableStatement;
class BlockStatement;

/// 
class Visitor {
public:
   virtual ~Visitor();

   virtual void onPreVisit(RootStatement& root) = 0;
   virtual void onPostVisit(RootStatement& root) = 0;
   virtual void onPreVisit(SelectStatement& select) = 0;
   virtual void onPostVisit(SelectStatement& select) = 0;
   virtual void onPreVisit(CreateTableStatement& createTable) = 0;
   virtual void onPostVisit(CreateTableStatement& createTable) = 0;
   virtual void onPreVisit(BlockStatement& block) = 0;
   virtual void onPostVisit(BlockStatement& block) = 0;
};


}

}
