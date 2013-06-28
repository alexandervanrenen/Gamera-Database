#pragma once

#include "Visitor.hpp"
#include <cstdint>

namespace harriet { class Environment; }

namespace dbi {

class SchemaManager;
class SegmentManager;

namespace script {

/// 
class PlanGenerationVisitor : public Visitor {
public:
   PlanGenerationVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager, harriet::Environment& environment);
   virtual ~PlanGenerationVisitor();

   virtual void onPreVisit(CreateTableStatement& createTable);
   virtual void onPreVisit(SelectStatement& select);
   virtual void onPreVisit(InsertStatement& insert);

private:
   SegmentManager& segmentManager;
   SchemaManager& schemaManager;
   harriet::Environment& environment;
};

}

}
