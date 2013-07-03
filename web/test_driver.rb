# Runs all tests from the test directory
# -----------------------------------------------
require 'test/unit'
require './validator.rb'
# -----------------------------------------------
for test_case in Dir["./test_cases/*.rb"] do
  require test_case
end
# -----------------------------------------------
