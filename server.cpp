#include "zmq/zmq.hpp"
#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "util/Utility.hpp"
#include "common/Config.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    if(!dbi::util::createFile("bin/swap_file", 100 * dbi::kPageSize)) {
         cout << "failed" << endl;
         throw;
      }
    dbi::Database db(dbi::DatabaseConfig{"bin/swap_file", 128}, true);
    db.executeQuery("select s.name from studenten s; select s.name from studenten s; {select s.name from studenten s; select s.name from studenten s;}");
    db.executeQuery("select s.matr, name from studenten,  studenten asd;");
    db.executeQuery("create table employee (id integer, \n country_id char(2), mgr_id integer, salery integer, first_name char(20), middle char(1), last_name char(20));");
    return 0;

    //  Socket to talk to clients
    zmq::context_t context(2);
    zmq::socket_t responder(context, ZMQ_REP);
    responder.bind("tcp://*:7854");

    while(true) {
        char buffer [10];
        responder.recv(buffer, 10, 0);
        printf("Received Hello\n");
        responder.send("World", 5, 0);
        sleep(1);
    }
    return 0;
}
