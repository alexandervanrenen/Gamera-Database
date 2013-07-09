#pragma once

#include <memory>
#include <vector>
#include <ios>
#include "btree/IndexKeySchema.hpp"

namespace harriet {
  class Value;
}

namespace dbi {

class IndexKeySchema;
class IndexKeyComparator;
class MergeSort;
class IndexKey;

/// A Facade for the actual sort engine, in this case it wraps away the MergeSort.
/// It provides the means to write and read tuples in a stream based fashion.
class ExternalSort {
public:
  ExternalSort(const IndexKeySchema& schema, const IndexKeyComparator& c);
  ~ExternalSort();

  void addTuple(const IndexKey& tuple);

  bool hasNextTuple() const;

  IndexKey readNextTuple();

  void readNextTuple(IndexKey& key);

  void sort();

  void rewind();

private:
  const IndexKeySchema schema;

  std::unique_ptr<MergeSort> sortEngine;

  std::unique_ptr<std::fstream> file;

  /// Just used for ensuring correct usage
  uint32_t tuplesWritten;
  uint32_t tuplesRead;
};

}
