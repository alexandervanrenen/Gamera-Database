# Forwards queries to gamera database
# -----------------------------------------------
require 'zmq'
# -----------------------------------------------
zmqContext = ZMQ::Context.new(1)
server = zmqContext.socket(ZMQ::REQ)
server.connect("tcp://127.0.0.1:7854")
server.send "!!!KILL"
server.close
zmqContext.close
# -----------------------------------------------
