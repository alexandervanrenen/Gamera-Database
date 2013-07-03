# Forwards queries to gamera database
# -----------------------------------------------
require './sqlite.rb'
require './gamera.rb'
require './validator.rb'
# -----------------------------------------------
class ConcatDB
  include Validator
  def initialize
    @gamera = GameraDB.new
    @sqlite = SqliteDB.new
  end

  def runQuery(query)
    gres = @gamera.runSingleQuery(query)
    sres = @sqlite.runQuery(query)
    vres = compare gres, sres
    "{\"gamera\":#{gres},\"sqlite\":#{sres},\"validator\":\"#{vres}\"}"
  end

  def closeConnection
    @gamera.closeConnection
  end
end
# -----------------------------------------------
