#include "SchemaManager.hpp"
#include "RelationSchema.hpp"
#include "segment_manager/SPSegment.hpp"
#include "util/Utility.hpp"
#include "schema/Common.hpp"
#include <iostream>

using namespace std;

namespace dbi {

SchemaManager::SchemaManager(SPSegment& storage)
: storage(storage)
{
   for(auto pageId=storage.beginPageId(); pageId!=storage.endPageId(); pageId++) {
      auto records = storage.getAllRecordsOfPage(*pageId);
      for(auto& record : records) {
         auto relation = util::make_unique<RelationSchema>(record.second);
         string name = relation->getName();
         relations.insert(make_pair(name, make_pair(record.first, move(relation))));
      }
   }
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
