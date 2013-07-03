# Forwards queries to gamera database
# -----------------------------------------------
require 'zmq'
require 'json'
# -----------------------------------------------
class GameraDB
  def initialize
    @zmqContext = ZMQ::Context.new(1)
    @server = @zmqContext.socket(ZMQ::REQ)
    @server.connect("tcp://127.0.0.1:7854")
  end

  def runQuery(query)
    @server.send query
    @server.recv
  end

  def runSingleQuery(query)
    raise "not exactly one statment" if query.count(";") != 1
    @server.send query
    msg = @server.recv
    parsed = JSON.parse msg
    if parsed["results"] != nil && parsed["results"].length!=0 && parsed["results"][0]["content"]!=nil && parsed["results"][0]["content"].length != 0
      "{\"error\":\"#{parsed["error"]}\", \"content\":" + parsed["results"][0]["content"].to_s + "}"
    else
      "{\"error\":\"#{parsed["error"]}\", \"content\":\"\"}"
    end
  end

  def closeConnection
    @server.close
    @zmqContext.close
  end

  def clear()
    @server.send "!!!CLEAR"
    @server.recv
  end
end
# -----------------------------------------------
