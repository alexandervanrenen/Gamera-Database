#pragma once

#include "common/Config.hpp"
#include "harriet/ScriptLanguage.hpp"
#include "harriet/Value.hpp"
#include "query_parser/Common.hpp"
#include <utility>
#include <vector>
#include <memory>
#include <set>

namespace harriet {
   class Value;
}

namespace dbi {

class Record;
class Signature;

namespace qopt { class ColumnAccessInfo; }

/// Just define interface for all operators
class Operator {
public:
   virtual const Signature& getSignature() const = 0;
   virtual void prepare(std::vector<harriet::Value>& globalRegister, const std::set<qopt::ColumnAccessInfo>& requiredColumns) = 0;
   virtual void dump(std::ostream& os, uint32_t lvl) const = 0;

   virtual void open() = 0;
   virtual bool next() = 0;
   virtual void close() = 0;

   virtual ~Operator();
};

}
