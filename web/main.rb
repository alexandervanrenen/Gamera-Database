# A REST server serving static files and passing all 'post '/query/execute'' to the logger and the gamera db
# -----------------------------------------------
require 'json'
require 'sinatra'
# -----------------------------------------------
# Deliver static content
set :static_cache_control => [:public, :max_age => 60]
get '/' do
  redirect to('/gamera.html')
end

# Handle queries
post '/query/compare' do
  request.body.rewind
  query = request.body.read
  require "./db_interface.rb"
  $db = ConcatDB.new # Create new one each time
  msg = $db.runQuery query
  $db.closeConnection
  msg
end

# Handle queries
post '/query/execute' do
  request.body.rewind
  query = request.body.read
  require "./gamera.rb"
  $db = GameraDB.new # Create new one each time
  msg = $db.runQuery query
  $db.closeConnection
  msg
end

# Handle queries
post '/query/clear' do
  request.body.rewind
  query = request.body.read
  require "./db_interface.rb"
  $db = ConcatDB.new # Create new one each time
  msg = $db.runQuery query
  $db.closeConnection
  msg
end

# Handle queries
post '/query/log' do
  request.body.rewind
  query = request.body.read
  require "./logger.rb" # Need to include here to trigger the END blocks
  $logger ||= LoggerDB.new
  $logger.logQuery query
end
# -----------------------------------------------
