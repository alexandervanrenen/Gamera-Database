#pragma once

#include "RootOperator.hpp"
#include <cstdint>
#include <memory>
#include <vector>

namespace harriet { class Value; }

namespace dbi {

namespace qopt { class GlobalRegister; }

class SPSegment;
class ProjectionOperator;
class RelationSchema;

/// Insert the tuples provided into the given table -- this is a root operator (can not have a parent)
class InsertOperator : public RootOperator {
public:
   InsertOperator(std::unique_ptr<ProjectionOperator> source, SPSegment& target, const RelationSchema& targetSchema, qopt::GlobalRegister& globalRegister);
   virtual ~InsertOperator();
   virtual void dump(std::ostream& os) const;

   void checkTypes() const throw(harriet::Exception);

   void execute();

private:
   std::unique_ptr<ProjectionOperator> source;
   SPSegment& target;
   const RelationSchema& targetSchema;
   qopt::GlobalRegister& globalRegister;
};

}
