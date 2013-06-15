#pragma once

#include "Visitor.hpp"
#include <cstdint>

namespace dbi {

class SchemaManager;
class SegmentManager;

namespace script {

/// 
class PlanGenerationVisitor : public Visitor {
public:
   PlanGenerationVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager);
   virtual ~PlanGenerationVisitor();

   virtual void onPreVisit(SelectStatement& select);
   virtual void onPreVisit(InsertStatement& insert);

private:
   SegmentManager& segmentManager;
   SchemaManager& schemaManager;
};

}

}
