#pragma once

#include <memory>
#include <string>

namespace harriet { class Environment; }

namespace dbi {

class Query;
class SegmentManager;
class SchemaManager;
class QueryResultCollection;

class QueryFacade {
public:
   QueryFacade(SegmentManager& segmentManager, SchemaManager& schemaManager);
   ~QueryFacade();

   std::unique_ptr<QueryResultCollection> executeQuery(const std::string& query, harriet::Environment& environment, bool showTree);

private:
   SegmentManager& segmentManager;
   SchemaManager& schemaManager;
};

}
