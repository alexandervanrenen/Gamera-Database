#include "SchemaManager.hpp"
#include "RelationSchema.hpp"
#include "segment_manager/SPSegment.hpp"
#include "operator/RecordScanOperator.hpp"
#include "util/Utility.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SchemaManager::SchemaManager(SPSegment& storage)
: storage(storage)
{
   dbi::RecordScanOperator scanner(storage);
   scanner.open();
   while(scanner.next()) {
      const pair<TupleId, Record>& entry = scanner.getRecord();
      auto relation = util::make_unique<RelationSchema>(entry.second);
      string name = relation->getName();
      relations.insert(make_pair(name, make_pair(entry.first, move(relation))));
   }
   scanner.close();
}

SchemaManager::~SchemaManager()
{
}

void SchemaManager::addRelation(RelationSchema& schema)
{
   assert(relations.count(schema.getName())==0);
   TupleId tid = storage.insert(schema.marschall());
   relations.insert(make_pair(schema.getName(), make_pair(tid, util::make_unique<RelationSchema>(schema))));
}

bool SchemaManager::hasRelation(const string& relationName) const
{
   return relations.count(relationName) == 1;
}

void SchemaManager::dropRelation(const string& relationName)
{
   assert(relations.count(relationName)==1);
   auto iter = relations.find(relationName);
   storage.remove(iter->second.first);
   relations.erase(iter);
}

const RelationSchema& SchemaManager::getRelation(const string& relationName) const
{
   assert(relations.count(relationName)==1);
   return *relations.find(relationName)->second.second;
}

}
