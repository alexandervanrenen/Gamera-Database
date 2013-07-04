#pragma once

namespace dbi {

namespace script {

class RootStatement;
class SelectStatement;
class CreateTableStatement;
class InsertStatement;
class DropTableStatement;
class BlockStatement;

/// 
class Visitor {
public:
   virtual ~Visitor();

   virtual void onPreVisit(RootStatement&) {};
   virtual void onPostVisit(RootStatement&) {};
   virtual void onPreVisit(SelectStatement&) {};
   virtual void onPostVisit(SelectStatement&) {};
   virtual void onPreVisit(CreateTableStatement&) {};
   virtual void onPostVisit(CreateTableStatement&) {};
   virtual void onPreVisit(InsertStatement&) {};
   virtual void onPostVisit(InsertStatement&) {};
   virtual void onPreVisit(DropTableStatement&) {};
   virtual void onPostVisit(DropTableStatement&) {};
   virtual void onPreVisit(BlockStatement&) {};
   virtual void onPostVisit(BlockStatement&) {};
};

}

}
