# Automatic generated test file
# -----------------------------------------------
require './validator.rb'
require 'test/unit'
# -----------------------------------------------
class LoggingTest_1 < Test::Unit::TestCase
  include Validator

  def test_all
    validate({"query"=>"select name, name, id from Person where id = 1;", "expected"=>"{\"error\":\"A runtime error: Unknown table in from clause: 'Person'.\",\"content\":\"\"}"})
    validate({"query"=>"create table Studenten (id integer, name char(20), age integer);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values (2, 'alex', 24);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values (2, 'alex' 24);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 41\",\"content\":\"\"}"})
    validate({"query"=>"insert into table Studenten values (2, 'alex', 24);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 18\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten (2, 'alex', 24);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 23\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values (2, 'alex', 2 4   );", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 44\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values (2, 'alex', 24.3   );", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values (2, 'alex', 24.3   )  ;", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"inser t into Studenten values (2, 'alex', 24.3   )  ;", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 6\",\"content\":\"\"}"})
    destroy
  end
end
# -----------------------------------------------