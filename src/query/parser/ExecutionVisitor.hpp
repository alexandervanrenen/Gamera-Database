#pragma once

#include "Visitor.hpp"
#include <cstdint>
#include <ios>
#include <vector>

namespace dbi {

class SchemaManager;
class SegmentManager;
class QueryResultCollection;

namespace script {

/// Visitor for execution all statements a given tree.
class ExecutionVisitor : public Visitor {
public:
   ExecutionVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager, QueryResultCollection& result);
   virtual ~ExecutionVisitor();

   virtual void onPreVisit(SelectStatement& select);
   virtual void onPreVisit(CreateTableStatement& createTable);
   virtual void onPreVisit(InsertStatement& insert);
   virtual void onPreVisit(DropTableStatement& dropTable);

private:
   SegmentManager& segmentManager;
   SchemaManager& schemaManager;
   QueryResultCollection& result;
};

}

}
