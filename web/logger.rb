# Logs all incomming queries to a new file in test_cases/
# -----------------------------------------------
require 'json'
# -----------------------------------------------
class LoggerDB
  def toZeroLeadingStr id, digists
    raise if id.to_s.length > digists
    res = id.to_s
    while res.length < digists
      res = "0" + res
    end
    res
  end

  def initialize
    existing_tests = Dir["./test_cases/*.rb"]
    if existing_tests.size == 0
      next_id = 0
    else
      next_id = existing_tests.sort.last[13..existing_tests.sort.last.size-4].to_i + 1
    end
    @file = File.new("test_cases/" + toZeroLeadingStr(next_id,10) + ".rb", "w")
    @file.syswrite "# Automatic generated test file\n"
    @file.syswrite "# -----------------------------------------------\n"
    @file.syswrite "require './validator.rb'\n"
    @file.syswrite "require 'test/unit'\n"
    @file.syswrite "# -----------------------------------------------\n"
    @file.syswrite "class LoggingTest_#{next_id} < Test::Unit::TestCase\n"
    @file.syswrite "  include Validator\n\n"
    @file.syswrite "  def test_all\n"
  end

  def logQuery input
    input = input.gsub(/\n/){" "}
    parsed = JSON.parse input
    query = parsed["query"]
    expected = parsed["expected"]
    raise "protocol error" if query==nil || expected==nil
    raise "empty query (.size==0) is not valid" if query.size==0
    raise "query is not terminated by an ';'" if query[query.size-1]!=";"
    @file.syswrite "    validate(" + parsed.to_s + ")\n"
  end

  def close
    @file.syswrite "    destroy\n"
    @file.syswrite "  end\n"
    @file.syswrite "end\n"
    @file.syswrite "# -----------------------------------------------"
    @file.close
  end
end
# -----------------------------------------------
END {
  if defined? $logger
    $logger.close
    $logger = nil
  end
}
