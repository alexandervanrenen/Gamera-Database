# 
# -----------------------------------------------
require 'json'
require "./gamera.rb"
# -----------------------------------------------
module Validator
  def compare gresIn, sresIn
    gres = JSON.parse gresIn
    sres = JSON.parse sresIn

    # Error in only one db both dbs => false .. error in both dbs => true
    if gres["error"].size==0 && sres["error"].size!=0
      return "An error in gamera, but not in sqlite"
    elsif gres["error"].size!=0 && sres["error"].size==0
      return "An error in sqlite, but not in gamera"
    elsif (gres["error"].size!=0 && sres["error"].size!=0)
      return ""
    end

    # Otherwise compare results
    if gres["content"]==nil || sres["content"] == gres["content"]
      ""
    else
      "The results of the databases are not equal"
    end
  end

  def validate input
    if not defined? @gamera
      @gamera ||= GameraDB.new
      @gamera.clear
    end

    res = @gamera.runSingleQuery input["query"]
    actual = JSON.parse(res)
    expexted = JSON.parse(input["expected"])

    # Check for errors
    if actual["error"].size==0 && expexted["error"].size!=0
      return assert_equal(expexted["error"], actual["error"])
    elsif actual["error"].size!=0 && expexted["error"].size==0
      return assert_equal(expexted["error"], actual["error"])
    elsif (actual["error"].size!=0 && expexted["error"].size!=0)
      return assert_equal(expexted["error"], actual["error"])
    end

    # Prapare nice content matching
    actual["query"] = input["query"]
    expexted["query"] = input["query"]
    assert_equal(expexted, actual)
  end

  def destroy
    @gamera.closeConnection if defined? @gamera
  end
end
# -----------------------------------------------
