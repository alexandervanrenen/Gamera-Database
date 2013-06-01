#include "zmq/zmq.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main(int argc, char** argv)
{
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
