# Automatic generated test file
# -----------------------------------------------
require './validator.rb'
require 'test/unit'
# -----------------------------------------------
class LoggingTest_2 < Test::Unit::TestCase
  include Validator

  def test_all
    validate({"query"=>"create table characters (id integer,\n                         name char(20),\n                         player_id integer,\n                         x float,\n                         y float,\n                         gold integer\n                        );", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"select name from characters;", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into characters values(1, 'flowly', 1, 81.28, 17.29, 3000000);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"select name from characters;", "expected"=>"{\"error\":\"\",\"content\":[[\"flowly\"]]}"})
    validate({"query"=>"select id, name from characters;", "expected"=>"{\"error\":\"\",\"content\":[[\"1\",\"flowly\"]]}"})
    validate({"query"=>"select id, name from characters where id = 1;", "expected"=>"{\"error\":\"\",\"content\":[[\"1\",\"flowly\"]]}"})
    validate({"query"=>"select id, name from characters where id = 2;", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"select id, name from characters where id+1 = 2;", "expected"=>"{\"error\":\"\",\"content\":[[\"1\",\"flowly\"]]}"})
    validate({"query"=>"select id+1 as id, name from characters where id+1 = 2;", "expected"=>"{\"error\":\"\",\"content\":[[\"2\",\"flowly\"]]}"})
    validate({"query"=>"select id+1 as id, name from characters where id+1*2 = 3;", "expected"=>"{\"error\":\"\",\"content\":[[\"2\",\"flowly\"]]}"})
    validate({"query"=>"select id+1 as id, name+'a' as name from characters where id+1*2 = 3;", "expected"=>"{\"error\":\"\",\"content\":[[\"2\",\"flowlya\"]]}"})
    validate({"query"=>"select id+1 as id, name+'a' from characters where id+1*2 = 3;", "expected"=>"{\"error\":\"A runtime error: No name provided for projection 'characters.name + a'.\",\"content\":\"\"}"})
    validate({"query"=>"select id+1 as id, name+'a' as name\nfrom characters\nwhere id+1*2 = 3;", "expected"=>"{\"error\":\"\",\"content\":[[\"2\",\"flowlya\"]]}"})
    destroy
  end
end
# -----------------------------------------------