# Automatic generated test file
# -----------------------------------------------
require './validator.rb'
require 'test/unit'
# -----------------------------------------------
class LoggingTest_3 < Test::Unit::TestCase
  include Validator

  def test_all
    validate({"query"=>"create table characters (id integer,\n                         name char(20),\n                         player_id integer,\n                         x float,\n                         y float,\n                         gold integer\n                        );", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into characters values(3, 'conscons', 3, 232.6, 45.765, 100);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"select * from characters;", "expected"=>"{\"error\":\"\",\"content\":[[\"3\",\"conscons\",\"3\",\"232.6\",\"45.765\",\"100\"]]}"})
    validate({"query"=>"select * from characters where id = 3;", "expected"=>"{\"error\":\"\",\"content\":[[\"3\",\"conscons\",\"3\",\"232.6\",\"45.765\",\"100\"]]}"})
    validate({"query"=>"select * from characters where id = 2;", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"select * from characters where id = 3;", "expected"=>"{\"error\":\"\",\"content\":[[\"3\",\"conscons\",\"3\",\"232.6\",\"45.765\",\"100\"]]}"})
    validate({"query"=>"drop table characters;", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"create table characters (id integer,\n                         name char(20),\n                         player_id integer,\n                         x float,\n                         y float,\n                         gold integer\n                        );", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"select * from characters;", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into characters values(2, 'sebseb', 2, 22.22, 22.22, 500);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"select * from  characters;", "expected"=>"{\"error\":\"\",\"content\":[[\"2\",\"sebseb\",\"2\",\"22.22\",\"22.22\",\"500\"]]}"})
    destroy
  end
end
# -----------------------------------------------