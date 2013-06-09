#include "SelectionSignature.hpp"
#include "schema/Common.hpp"
#include "harriet/Expression.hpp"
#include "harriet/Environment.hpp"
#include <sstream>

using namespace std;

namespace dbi {

namespace {
   ColumnIdentifier toColumnIdentifier(const string& str)
   {
      ColumnIdentifier id;
      size_t splitPos = str.find('.');
      id.tableIdentifier = str.substr(0, splitPos);
      id.columnIdentifier = (splitPos==string::npos?"":str.substr(splitPos+1, str.size()));
      return id;
   }
}

SelectionSignature::SelectionSignature(const Signature& source, std::unique_ptr<harriet::Expression> expression)
{
   for(auto& attribute : source.getAttributes())
      attributes.push_back(attribute);

   // Find free variables
   vector<const harriet::Variable*> freeVariables = expression->getAllVariables();
   for(auto iter : freeVariables) {
      ColumnIdentifier c = toColumnIdentifier(iter->getIdentifier());
      if(hasAttribute(c.tableIdentifier, c.columnIdentifier)) {
         variableMapping.push_back(VariableMapping{iter->getIdentifier(), getAttributeIndex(c.tableIdentifier, c.columnIdentifier)});
      } else {
         ostringstream os;
         dump(os);
         throw harriet::Exception{"unknown identifier: '" + iter->getIdentifier() + "' \ncandidates are: " + (os.str().size()==0?"<none>":os.str())};
      }
   }

   this->expression = move(expression);
}

bool SelectionSignature::fullfillsPredicates(const vector<unique_ptr<harriet::Value>>& tuple)
{
   harriet::Environment env;
   for(auto& iter : variableMapping)
      env.add(iter.name, tuple[iter.position]->evaluate());
   auto result = expression->evaluate(env);
   return reinterpret_cast<harriet::BoolValue&>(*result).result;
}

}
