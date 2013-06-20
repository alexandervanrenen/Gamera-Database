#include "common/Config.hpp"
#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "util/Utility.hpp"
#include "zmq/zmq.hpp"
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace std;

int main(int, char**)
{
   if(!dbi::util::createFile("bin/swap_file", 100 * dbi::kPageSize)) {
      cout << "failed" << endl;
      throw;
   }
   dbi::Database db(dbi::DatabaseConfig{"bin/swap_file", 128}, true);

   // Socket to talk to clients
   zmq::context_t context(2);
   zmq::socket_t responder(context, ZMQ_REP);
   responder.bind("tcp://*:7854");

   while(true) {
      char buffer[4096];
      memset(buffer, '\0', 4096);
      responder.recv(buffer, 4096, 0);
      string query(buffer);
      cout << ":> " << query << endl;
      db.executeQuery(query);
      responder.send("ok", 2, 0);
   }
   return 0;
}
