#include "IndexKeyComparator.hpp"

namespace dbi {

IndexKeyComparator::IndexKeyComparator(IndexKeySchema& schema)
: schema(schema)
{
}

bool IndexKeyComparator::less(const IndexKey& lhs, const IndexKey& rhs) const
{
}

bool IndexKeyComparator::less(const IndexKey& lhs, const char* rhs) const
{
}

bool IndexKeyComparator::less(const char* lhs, const IndexKey& rhs) const
{
}

bool IndexKeyComparator::less(const char* lhs, const char* rhs) const
{
}

bool IndexKeyComparator::equal(const IndexKey& lhs, const IndexKey& rhs) const
{
}

bool IndexKeyComparator::equal(const IndexKey& lhs, const char* rhs) const
{
}

bool IndexKeyComparator::equal(const char* lhs, const IndexKey& rhs) const
{
}

bool IndexKeyComparator::equal(const char* lhs, const char* rhs) const
{
}

}
