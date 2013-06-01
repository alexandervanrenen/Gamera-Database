#include "zmq/zmq.hpp"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main(int argc, char** argv)
{
    //  Socket to talk to clients
    zmq::context_t context(2);
    zmq::socket_t responder(context, ZMQ_REQ);
    responder.connect("tcp://127.0.0.1:7854");

    while(true) {
        char buffer [10];
        responder.send("alex", 4, 0);
        printf("Received Hello\n");
        responder.recv(buffer, 10, 0);
        sleep(1);
    }
    return 0;
}
