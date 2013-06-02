#pragma once

#include <vector>
#include <cstdint>
#include <iostream>

namespace dbi {

namespace script {

struct Statement {
   virtual void dump() = 0;
};

struct SelectStatement : public Statement {

   struct Selector { std::string tableIdentifier; std::string columnIdentifier; };
   std::vector<Selector> selectors;

   struct Source { std::string tableIdentifier; std::string alias; };
   std::vector<Source> sources;

   virtual void dump() {
      std::cout << "---------" << std::endl;
      std::cout << "select " << std::endl;
      for(auto iter : selectors)
         std::cout << "    " << iter.tableIdentifier << " " << iter.columnIdentifier << std::endl;
      std::cout << "from " << std::endl;
      for(auto iter : sources)
         std::cout << "    " << iter.tableIdentifier << " " << iter.alias << std::endl;
      std::cout << "---------" << std::endl;
   }
};

struct CreateStatement : public Statement {

   struct Attribute {
      std::string name;
      std::string type;
      bool notNull;
      void dump() {std::cout << name << " " << type << " " << notNull << std::endl;}
   };
   std::string name;
   std::vector<Attribute> attributes;
   std::vector<unsigned> primaryKey;

   virtual void dump() {
      std::cout << "---------" << std::endl;
      std::cout << "create " << name << std::endl;
      for(auto& iter : attributes)
         iter.dump();
      std::cout << "---------" << std::endl;
   }
};

}

}
