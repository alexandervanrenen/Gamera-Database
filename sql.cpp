#include "common/Config.hpp"
#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "query/result/QueryResultCollection.hpp"
#include "util/Utility.hpp"
#include <iostream>
#include <string>
#include <csignal>

using namespace std;

namespace {
unique_ptr<dbi::Database> db;
void signal_handler(int signal)
{
   db = nullptr; // TODO .. does not seem save
   cout << "saved database" << endl;
   exit(0);
}
}

class CommandLineInterface {
   dbi::Database &db;
   bool showPrompt = true;
   string buffer;
public:
   CommandLineInterface(dbi::Database &db)
           : db(db)
   {
      signal(SIGINT, signal_handler);
   }

   void run()
   {
      while (cin.good()) {
         if (showPrompt)
            cout << ":>" << flush;

         string line;
         getline(cin, line);
         buffer += line + " ";
         int pos = buffer.find(';');
         if (pos != string::npos) {
            string statement = buffer.substr(0, pos + 1);
            buffer = buffer.substr(pos + 1);
            auto result = db.executeQuery(statement);
            result->print(cout);
         }
      }
      cout << endl;
   }
};

int main(int argc, char **argv)
{
   string dbPath = "bin/swap_file";
   if (argc == 2) {
      dbPath = argv[1];
   }

   if (dbi::util::fileExists(dbPath)) {
      cout << "loading database from file: '" << dbPath << "' .. " << flush;
      db = dbi::util::make_unique<dbi::Database>(dbi::DatabaseConfig {dbPath, 256}, false);
   } else {
      cout << "creating swap file: '" << dbPath << "' .. " << flush;
      if (!dbi::util::createFile(dbPath, 512 * dbi::kPageSize)) {
         cout << "failed" << endl;
         return -1;
      }
      db = dbi::util::make_unique<dbi::Database>(dbi::DatabaseConfig {dbPath, 256}, true);
   }
   cout << "done =)" << endl;

   CommandLineInterface cli(*db);
   cli.run();

   return 0;
}
