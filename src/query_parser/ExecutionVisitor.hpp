#pragma once

#include "Visitor.hpp"
#include <cstdint>

namespace dbi {

class SchemaManager;
class SegmentManager;

namespace script {

/// 
class ExecutionVisitor : public Visitor {
public:
   ExecutionVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager, bool verbose = false);
   virtual ~ExecutionVisitor();

   virtual void onPreVisit(RootStatement& root);
   virtual void onPostVisit(RootStatement& root);
   virtual void onPreVisit(SelectStatement& select);
   virtual void onPostVisit(SelectStatement& select);
   virtual void onPreVisit(CreateTableStatement& createTable);
   virtual void onPostVisit(CreateTableStatement& CreateTableStatemente);
   virtual void onPreVisit(InsertStatement& insert);
   virtual void onPostVisit(InsertStatement& insert);
   virtual void onPreVisit(BlockStatement& block);
   virtual void onPostVisit(BlockStatement& block);

private:
   SegmentManager& segmentManager;
   SchemaManager& schemaManager;
   bool verbose;
};

}

}
