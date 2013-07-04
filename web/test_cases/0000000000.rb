# Automatic generated test file
# -----------------------------------------------
require './validator.rb'
require 'test/unit'
# -----------------------------------------------
class LoggingTest_0 < Test::Unit::TestCase
  include Validator

  def test_all
    validate({"query"=>"select name, name, id from Person where id = 1;", "expected"=>"{\"error\":\"A runtime error: Unknown table in from clause: 'Person'.\",\"content\":\"\"}"})
    validate({"query"=>"create table Studenten (id integer, name char(20), age integer);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values(2, 3, 4);", "expected"=>"{\"error\":\"A runtime error: Insert into Studenten: invalid conversion from 'integer' to 'character (20)' for argument 1.\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values('al\", 3, 4);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 43\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values(\"al\", 3, 4);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 30\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten value('al', 3, 4);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 28\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten value ('al', 3, 4);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 28\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values'al', 3, 4);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 29\",\"content\":\"\"}"})
    validate({"query"=>"insertinto Studenten values('al', 3, 4);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 7\",\"content\":\"\"}"})
    validate({"query"=>"insert into Sudenten values('al', 3, 4);", "expected"=>"{\"error\":\"A runtime error: Insert into unknown table: 'Sudenten'.\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studentens values('al', 3, 4);", "expected"=>"{\"error\":\"A runtime error: Insert into unknown table: 'Studentens'.\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studentenvalues('al', 3, 4);", "expected"=>"{\"error\":\"A parser error occurred: line 1 column 28\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values(2, 3, 4);", "expected"=>"{\"error\":\"A runtime error: Insert into Studenten: invalid conversion from 'integer' to 'character (20)' for argument 1.\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values(2, 'alex', 4);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values(3, 'cons', 23);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert into Studenten values(1, 'seb', 22);", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    validate({"query"=>"insert \n into  Studenten   values (4,'asd' , 22 )\n ;", "expected"=>"{\"error\":\"\",\"content\":\"\"}"})
    destroy
  end
end
# -----------------------------------------------