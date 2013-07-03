# Forwards queries to sqlite database
# -----------------------------------------------
require './json_util.rb'
require 'sqlite3'
# -----------------------------------------------
class SqliteDB
  def initialize
    @db = SQLite3::Database.new("test.db")
  end

  def runQuery(query)
    begin
      qres = @db.execute query
    rescue
      "{\"error\":\"an error occurred\"}"
    else
      "{\"error\":\"\", \"content\":" + JsonHelper.serializeArrayOfArrays(qres) + "}"
    end
  end

  def clear
    @db = nil
    File.delete("test.db")
    @db = SQLite3::Database.new("test.db")
  end
end
# -----------------------------------------------
