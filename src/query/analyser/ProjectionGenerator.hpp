#pragma once

#include <memory>
#include <vector>

namespace harriet { class Environment; class Expression; }

namespace dbi {

namespace qopt { class Projection; class TableAccessInfo; class GlobalRegister; }

namespace qgen {

class ProjectionGenerator {
public:
   ProjectionGenerator(harriet::Environment& environment, qopt::GlobalRegister& globalRegister);

   std::vector<std::unique_ptr<qopt::Projection>> createProjections(std::vector<std::pair<std::string, std::unique_ptr<harriet::Expression>>>& expressions) const;

private:
   harriet::Environment& environment;
   qopt::GlobalRegister& globalRegister;

   std::unique_ptr<qopt::Projection> createProjection(std::pair<std::string, std::unique_ptr<harriet::Expression>> expression) const;
};

}

}
