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
   PlanGenerationVisitor(SchemaManager& schemaManager, SegmentManager& segmentManager, bool verbose = false);
   virtual ~PlanGenerationVisitor();

   virtual void onPreVisit(InsertStatement& insert);
   virtual void onPostVisit(InsertStatement& insert);

private:
   SchemaManager& schemaManager;
   SegmentManager& segmentManager;
   bool verbose;
};

}

}
