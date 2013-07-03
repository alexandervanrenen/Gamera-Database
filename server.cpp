#include "common/Config.hpp"
#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "query/result/QueryResultCollection.hpp"
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
   auto db = dbi::util::make_unique<dbi::Database>(dbi::DatabaseConfig{"bin/swap_file", 128}, true);

   // Socket to talk to clients
   zmq::context_t context(2);
   zmq::socket_t responder(context, ZMQ_REP);
   responder.bind("tcp://*:7854");

   while(true) {
      zmq::message_t msg;
      responder.recv(&msg);
      string query((char*)msg.data(), msg.size());
      cout << query << endl;
      if(query == "!!!CLEAR") {
         db = dbi::util::make_unique<dbi::Database>(dbi::DatabaseConfig{"bin/swap_file", 128}, true);
         string str = "ok";
         msg.rebuild(str.size());
         memcpy(msg.data(), str.data(), str.size());
         responder.send(msg);
         continue;
      }

      auto result = db->executeQuery(query);
      ostringstream os;
      result->toJSON(os);

      string str = os.str();
      msg.rebuild(str.size());
      memcpy(msg.data(), str.data(), str.size());
      responder.send(msg);
   }
   return 0;
}
