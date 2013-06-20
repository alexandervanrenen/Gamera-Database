#include "zmq/zmq.hpp"
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

using namespace std;

int main(int, char**)
{
    //  Socket to talk to clients
    zmq::context_t context(2);
    zmq::socket_t responder(context, ZMQ_REQ);
    responder.connect("tcp://127.0.0.1:7854");

    while(true) {
        string query = "";
        getline(cin, query, ';');
        cin.clear();
        query = query + ";";
        responder.send(query.data(), query.size(), 0);

        char buffer [10];
        responder.recv(buffer, 10, 0);
        cout << buffer << endl;
    }
    return 0;
}
