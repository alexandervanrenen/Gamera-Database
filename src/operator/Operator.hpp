#pragma once

#include "common/Config.hpp"
#include "harriet/ScriptLanguage.hpp"
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
   virtual const RelationSchema& getSignature() const = 0;
   virtual void checkTypes() const throw(harriet::Exception) = 0;

   virtual void open() = 0;
   virtual bool next() = 0;
   virtual std::vector<std::unique_ptr<harriet::Value>> getOutput() = 0;
   virtual void close() = 0;

   virtual ~Operator();
};

}
