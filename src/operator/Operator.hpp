#pragma once

#include "common/Config.hpp"
#include <utility>
#include <vector>
#include <memory>

namespace harriet {
   class Value;
}

namespace dbi {

class Record;
class RelationSchema;

/// Just define interface for all operators
class Operator {
public:
   /// During plan creation
   virtual const RelationSchema& getSignatur() const = 0;

   /// During transaction execution
   virtual void open() = 0;
   virtual bool next() = 0;
   virtual std::vector<std::unique_ptr<harriet::Value>> getOutput() = 0;
   virtual void close() = 0;

   virtual ~Operator();
};

}
