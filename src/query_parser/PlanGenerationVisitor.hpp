#pragma once

#include "Visitor.hpp"
#include <cstdint>

namespace dbi {

class SchemaManager;

namespace script {

/// 
class PlanGenerationVisitor : public Visitor {
public:
   PlanGenerationVisitor(SchemaManager& schemaManager, bool verbose = false);
   virtual ~PlanGenerationVisitor();

   virtual void onPreVisit(InsertStatement& insert);
   virtual void onPostVisit(InsertStatement& insert);

private:
   SchemaManager& schemaManager;
   bool verbose;
};

}

}
