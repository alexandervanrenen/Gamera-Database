#pragma once

namespace dbi {

class Record;

/// Just define interface for all operators
class Operator {
public:
   virtual void open() = 0;

   virtual bool next() = 0;

   virtual const Record& getOutput() = 0;

   virtual void close() = 0;

   virtual ~Operator();
};

}
