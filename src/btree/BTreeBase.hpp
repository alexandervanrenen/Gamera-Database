#ifndef BTREEBASE_HPP
#define BTREEBASE_HPP

#include "common/Config.hpp"

namespace dbi {

const uint64_t PAGESIZE = kPageSize;//4*1024;
//typedef uint64_t PageId;
//typedef uint64_t TID;
typedef TupleId TID;

}

#endif
