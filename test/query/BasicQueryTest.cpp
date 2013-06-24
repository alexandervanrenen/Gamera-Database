#include "gtest/gtest.h"
#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "test/TestConfig.hpp"
#include "query/result/QueryResult.hpp"
#include "query/result/QueryResultCollection.hpp"
#include "harriet/Value.hpp"
#include "util/Utility.hpp"
#include "QueryTestUtil.hpp"
#include <set>

using namespace std;
using namespace dbi;

TEST(BasicQueryTest, GoodRoundTrip)
{
   DatabaseConfig config{kSwapFileName, kSwapFilePages};

   // Start database
   Database db(config, true);
   ASSERT_TRUE(db.executeQuery("create table Persons (name char(20), job char(20));")->good());
   ASSERT_TRUE(db.executeQuery("insert into Persons values('Lenard', 'Physics');")->good());
   auto queryResultCollection = db.executeQuery("select name from Persons;");

   // Check query
   vector<vector<harriet::Value>> result(1);
   result[0].emplace_back(harriet::Value::createCharacter(string("Lenard"), 10));
   ASSERT_TRUE(validateResult(move(queryResultCollection), result));
}

TEST(BasicQueryTest, GoodRoundTripWithRestart)
{
   DatabaseConfig config{kSwapFileName, kSwapFilePages};

   // Start database
   {
      Database db(config, true);
      ASSERT_TRUE(db.executeQuery("create table Persons (name char(20), job char(20));")->good());
      ASSERT_TRUE(db.executeQuery("insert into Persons values('Lenard', 'Physics');")->good());
      auto queryResultCollection = db.executeQuery("select name from Persons;");

      // Check query
      vector<vector<harriet::Value>> result(1);
      result[0].emplace_back(harriet::Value::createCharacter(string("Lenard"), 10));
      ASSERT_TRUE(validateResult(move(queryResultCollection), result));
   }

   // Restart database
   {
      Database db(config, false);
      auto queryResultCollection = db.executeQuery("select name from Persons;");

      // Check query
      vector<vector<harriet::Value>> result(1);
      result[0].emplace_back(harriet::Value::createCharacter(string("Lenard"), 10));
      ASSERT_TRUE(validateResult(move(queryResultCollection), result));
   }
}
