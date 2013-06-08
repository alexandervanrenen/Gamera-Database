#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include "gflags/gflags.h"
#include <iostream>
#include <string>

using namespace std;

DEFINE_uint64(mainMemory  , 256              , "number of pages in main memory");
DEFINE_uint64(discMemory  , 512              , "number of pages on disc");
DEFINE_string(swapfile    , "bin/swap_file"  , "swap file name for the database");
DEFINE_string(file        , ""               , "file to process queries from");
DEFINE_bool  (restart     , false            , "restart an existing database");

int main(int argc, char** argv)
{
   // init gflags and read argv
   google::SetUsageMessage("gamera database driver");
   google::SetVersionString("0.1");
   google::ParseCommandLineFlags(&argc, &argv, true);

   if(FLAGS_file == "") {
      cout << "usage: ./bin/driver --file [query-file]" << endl;
      return -1;
   }

   if(!FLAGS_restart) {
      cout << "creating swap file .. ";
      cout.flush();
      if(!dbi::util::createFile(FLAGS_swapfile, FLAGS_discMemory * dbi::kPageSize)) {
         cout << "failed" << endl;
         throw;
      }
      cout << "done =)" << endl;
   } else {
      cout << "using existing swap file" << endl;
   }

   dbi::Database db(dbi::DatabaseConfig{FLAGS_swapfile, FLAGS_mainMemory}, !FLAGS_restart);
   string query = dbi::util::loadFileToMemory(FLAGS_file);
   db.executeQuery(query);

   return 0;
}
