#include "common/Config.hpp"
#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "query/result/QueryResultCollection.hpp"
#include "util/Utility.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv)
{
   if(argc != 2) {
      cout << "usage: ./bin/driver [query-file]" << endl;
      return -1;
   }

   cout << "creating swap file .. " << flush;
   if(!dbi::util::createFile("bin/swap_file", 512 * dbi::kPageSize)) {
      cout << "failed" << endl;
      return -1;
   }
   cout << "done =)" << endl;

   dbi::Database db(dbi::DatabaseConfig{"bin/swap_file", 256}, true);
   string query = dbi::util::loadFileToMemory(argv[1]);
   auto result = db.executeQuery(query);
   result->print(cout);

   return 0;
}
