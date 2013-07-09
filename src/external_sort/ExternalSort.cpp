#include "ExternalSort.hpp"
#include "harriet/Value.hpp"
#include "MergeSort.hpp"
#include "btree/IndexKey.hpp"
#include "btree/IndexKeyComparator.hpp"
#include "common/Config.hpp"
#include "util/Utility.hpp"
#include <fstream>
#include <cassert>

using namespace std;

namespace dbi {

ExternalSort::ExternalSort(const IndexKeySchema& schema, const IndexKeyComparator& c)
: schema(schema)
, sortEngine(util::make_unique<MergeSort>(kSortPageSize, kSortMemorySize, schema, c))
, file(util::make_unique<fstream>(kSortInputFileName.c_str(), ios::out))
, tuplesWritten(0)
, tuplesRead(-1)
{
   assert(file->good());
}

ExternalSort::~ExternalSort()
{
}

void ExternalSort::addTuple(const IndexKey& tuple)
{
   assert(tuple.bytes() == schema.bytes());
   vector<char> buffer(schema.bytes());
   tuple.writeToMem(buffer.data());
   file->write(buffer.data(), buffer.size());
   tuplesWritten++;
}

IndexKey ExternalSort::readNextTuple()
{
   assert(tuplesRead < tuplesWritten);
   tuplesRead++;
   vector<char> buffer(schema.bytes());
   file->read(buffer.data(), buffer.size());
   return IndexKey::readFromMemory(buffer.data(), schema);
}

void ExternalSort::readNextTuple(IndexKey& key)
{
   assert(tuplesRead < tuplesWritten);
   tuplesRead++;
   vector<char> buffer(schema.bytes());
   file->read(buffer.data(), buffer.size());
   key.readFromMemory(buffer.data());
}

void ExternalSort::sort()
{
   file->close();
   if(sortEngine->externalsort(kSortInputFileName, kSortOutputFileName) != 0)
      throw;
   file = util::make_unique<fstream>(kSortOutputFileName.c_str());
   assert(file->good());
   tuplesRead = 0;
}

bool ExternalSort::hasNextTuple() const
{
   return tuplesRead != tuplesWritten;
}

void ExternalSort::rewind()
{
   file = util::make_unique<fstream>(kSortOutputFileName);
   tuplesRead = 0;
}

}
