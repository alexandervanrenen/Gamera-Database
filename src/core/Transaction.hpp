#pragma once

namespace dbi {

class Transaction {
public:
   Transaction(Statement& statement);


private:
   Statement& statement;
};

}
