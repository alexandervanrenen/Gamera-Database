#include "PlanGenerationVisitor.hpp"
#include "schema/SchemaManager.hpp"
#include "operator/SingleRecordOperator.hpp"
#include "operator/InsertOperator.hpp"
#include "util/Utility.hpp"

namespace dbi {

namespace script {

PlanGenerationVisitor::PlanGenerationVisitor(SchemaManager& schemaManager, bool verbose)
: schemaManager(schemaManager)
, verbose(verbose)
{
}

PlanGenerationVisitor::~PlanGenerationVisitor()
{
}

void PlanGenerationVisitor::onPreVisit(InsertStatement& insert)
{
   // auto source = util::make_unique<SingleRecordOperator>(insert.values, );
}

void PlanGenerationVisitor::onPostVisit(InsertStatement& insert)
{
   
}

}

}
