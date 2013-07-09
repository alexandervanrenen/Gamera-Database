#include "PlanGenerationVisitor.hpp"
#include "harriet/Environment.hpp"
#include "harriet/Expression.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "query/operator/PrintOperator.hpp"
#include "query/operator/ProjectionOperator.hpp"
#include "query/operator/SortOperator.hpp"
#include "query/optimizer/ChainOptimizer.hpp"
#include "query/analyser/DependencyAnalyser.hpp"
#include "query/util/Predicate.hpp"
#include "query/util/Projection.hpp"
#include "query/analyser/PredicateGenerator.hpp"
#include "query/analyser/ProjectionGenerator.hpp"
#include "query/util/TableAccessInfo.hpp"
#include "schema/SchemaManager.hpp"
#include "segment_manager/SegmentManager.hpp"
#include "Statement.hpp"
#include "util/Utility.hpp"
#include "query/util/GlobalRegister.hpp"
#include "segment_manager/SPSegment.hpp"
#include <sstream>
#include <algorithm>

using namespace std;

namespace dbi {

namespace script {

PlanGenerationVisitor::PlanGenerationVisitor(SegmentManager& segmentManager, SchemaManager& schemaManager, harriet::Environment& environment)
: segmentManager(segmentManager)
, schemaManager(schemaManager)
, environment(environment)
{
}

PlanGenerationVisitor::~PlanGenerationVisitor()
{
}


void PlanGenerationVisitor::onPreVisit(CreateTableStatement& createTable)
{
   // Check if table already exists
   if(schemaManager.hasRelation(createTable.tableName))
      throw harriet::Exception("Can not create table with already existing name: '" + createTable.tableName + "'.");

   // Check that the unique constraints are good
   for(auto uniqueConstraint : createTable.uniqueColumns) {
      set<string> check;
      for(auto column : uniqueConstraint) {
         // Check that column names in the unique constraint are unqiue i.e. unique(id,id) is not valid
         if(check.count(column) > 0)
            throw harriet::Exception("Found duplicate column name in unique constraint: '" + column + "'"); else
            check.insert(column);

         // Check that the column names exist in the table
         if(createTable.attributes.end() == find_if(createTable.attributes.begin(), createTable.attributes.end(), [&column](const AttributeDeclaration& val){return val.name == column;}))
            throw harriet::Exception("Unknown column in unique constraint: '" + column + "'");
      }
   }

   // Check for minimal tuple size
   uint32_t size = 0;
   for(auto& iter : createTable.attributes)
      size += iter.type.length;
   if(size < 8)
      throw harriet::Exception("Table '" + createTable.tableName + "' has a row size of " + to_string(size) + ". Minimum is 8, sorry.");
}

void PlanGenerationVisitor::onPreVisit(SelectStatement& select)
{
   // Build a vector containing all TableAccessInfos
   for(uint32_t i=0; i<select.sources.size(); i++) {
      if(!schemaManager.hasRelation(select.sources[i].tableName))
         throw harriet::Exception("Unknown table in from clause: '" + select.sources[i].tableName + "'.");
      auto& relationSchema = schemaManager.getRelation(select.sources[i].tableName);
      string qualifier = select.sources[i].tableQualifier!=""?select.sources[i].tableQualifier:select.sources[i].tableName;
      auto& segment = segmentManager.getSPSegment(relationSchema.getRelationSegmentId());
      select.tableAccessVec.push_back(qopt::TableAccessInfo{relationSchema, segment, qualifier, i});
   }

   // Handle asterisk
   if(select.projections.empty())
      for(auto& table : select.tableAccessVec)
         for(auto& column : table.schema.getAttributes())
            select.projections.push_back(make_pair(column.name, harriet::Expression::createVariableExpression(column.name)));

   // Build global register -- contains all columns needed by the select (also does name resolution)
   qgen::DependencyAnalyser dependencyAnalyser(environment, select.tableAccessVec);
   select.globalRegister = dependencyAnalyser.createGlobalRegister(select.projections, select.conditions, select.orderBy);

   // Select clause -- Build projections from selection-expressions (may add entries to the global register)
   qgen::ProjectionGenerator projectionGenerator(environment, *select.globalRegister);
   vector<std::unique_ptr<qopt::Projection>> projections = projectionGenerator.createProjections(select.projections);

   // Where clause -- Build predicates from condition-expressions
   qgen::PredicateGenerator predicateGenerator(environment, *select.globalRegister);
   vector<std::unique_ptr<qopt::Predicate>> predicates = predicateGenerator.createPredicates(select.conditions);

   // Let the optimizer build a nice access tree from the table access' and the predicates
   qopt::ChainOptimizer opty(environment, *select.globalRegister);
   auto plan = opty.optimize(select.tableAccessVec, predicates);

   // Finally: Add the projection and sort order. Then we are good to go =)
   if(!select.orderBy.empty())
      plan = util::make_unique<SortOperator>(move(plan), select.orderBy, *select.globalRegister);
   auto projection = util::make_unique<ProjectionOperator>(move(plan), move(projections), *select.globalRegister);
   select.queryPlan = util::make_unique<PrintOperator>(move(projection), *select.globalRegister);
}

void PlanGenerationVisitor::onPreVisit(InsertStatement& insert)
{
   // Check that table already exists
   if(!schemaManager.hasRelation(insert.tableName))
      throw harriet::Exception("Insert into unknown table: '" + insert.tableName + "'.");

   // Simple case: A single tuple is provided => just insert it
   {
      // Get target relation
      auto& targetSchema = schemaManager.getRelation(insert.tableName);

      // See if we can insert the provided types into the table
      if(insert.values.size() != targetSchema.getAttributes().size())
         throw harriet::Exception{"Insert " + targetSchema.getName() + ": expected " + to_string(targetSchema.getAttributes().size()) + " arguments, " + to_string(insert.values.size()) + " provided."};
      for(uint32_t i=0; i<insert.values.size(); i++)
         if(!harriet::isImplicitCastPossible(insert.values[i].type, targetSchema.getAttributes()[i].type))
            throw harriet::Exception{"Insert into " + targetSchema.getName() + ": invalid conversion from '" + insert.values[i].type.str() + "' to '" + targetSchema.getAttributes()[i].type.str() + "' for argument " + to_string(i) + "."};
   }
}

void PlanGenerationVisitor::onPreVisit(DropTableStatement& dropTable)
{
   // Check that table already exists
   if(!schemaManager.hasRelation(dropTable.tableName))
      throw harriet::Exception("Drop unknown table: '" + dropTable.tableName + "'.");
}

}

}
