#pragma once

#include <cstdint>
#include "IndexKeySchema.hpp"

namespace dbi {

class IndexKey;
class IndexKeySchema;

class IndexKeyComparator {
public:
   IndexKeyComparator(const IndexKeySchema& schema);

   bool less(const IndexKey& lhs, const IndexKey& rhs) const;
   bool less(const IndexKey& lhs, const char* rhs) const;
   bool less(const char* lhs, const IndexKey& rhs) const;
   bool less(const char* lhs, const char* rhs) const;

   bool equal(const IndexKey& lhs, const IndexKey& rhs) const;
   bool equal(const IndexKey& lhs, const char* rhs) const;
   bool equal(const char* lhs, const IndexKey& rhs) const;
   bool equal(const char* lhs, const char* rhs) const;

private:
   IndexKeySchema schema;
};

}
