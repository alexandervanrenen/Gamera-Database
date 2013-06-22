#include "gtest/gtest.h"
#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "test/TestConfig.hpp"
#include "query/result/QueryResult.hpp"
#include "query/result/QueryResultCollection.hpp"
#include "harriet/Value.hpp"
#include <set>

using namespace std;
using namespace dbi;

TEST(Query, GoodRoundTrip)
{
   DatabaseConfig config{kSwapFileName, kSwapFilePages};

   // Start database
   Database db(config, true);
   ASSERT_TRUE(db.executeQuery("create table Persons (name char(20), job char(20));")->good());
   ASSERT_TRUE(db.executeQuery("insert into Persons values('Lenard', 'Physics');")->good());
   auto queryResultCollection = db.executeQuery("select name from Persons;");

   // Check query
   ASSERT_TRUE(queryResultCollection->getAsSelect() != nullptr); // No error during query execution
   auto& selectResult = *queryResultCollection->getAsSelect(); // Get this select
   ASSERT_TRUE(selectResult.result.size() == 1); // Only one row is selected
   ASSERT_TRUE(selectResult.result[0].size() == 1); // Only one Column is selected
   ASSERT_TRUE(selectResult.result[0][0] == harriet::Value::createCharacter(string("Lenard"), 10)); // The column is Lenard
}

TEST(Query, GoodRoundTripWithRestart)
{
   DatabaseConfig config{kSwapFileName, kSwapFilePages};

   // Start database
   {
      Database db(config, true);
      ASSERT_TRUE(db.executeQuery("create table Persons (name char(20), job char(20));")->good());
      ASSERT_TRUE(db.executeQuery("insert into Persons values('Lenard', 'Physics');")->good());
      auto queryResultCollection = db.executeQuery("select name from Persons;");

      // Check query
      ASSERT_TRUE(queryResultCollection->getAsSelect() != nullptr); // No error during query execution
      auto& selectResult = *queryResultCollection->getAsSelect(); // Get this select
      ASSERT_TRUE(selectResult.result.size() == 1); // Only one row is selected
      ASSERT_TRUE(selectResult.result[0].size() == 1); // Only one Column is selected
      ASSERT_TRUE(selectResult.result[0][0] == harriet::Value::createCharacter(string("Lenard"), 10)); // The column is Lenard
   }

   // Restart database
   {
      Database db(config, false);
      auto queryResultCollection = db.executeQuery("select name from Persons;");

      // Check query
      ASSERT_TRUE(queryResultCollection->getAsSelect() != nullptr); // No error during query execution
      auto& selectResult = *queryResultCollection->getAsSelect(); // Get this select
      ASSERT_TRUE(selectResult.result.size() == 1); // Only one row is selected
      ASSERT_TRUE(selectResult.result[0].size() == 1); // Only one Column is selected
      ASSERT_TRUE(selectResult.result[0][0] == harriet::Value::createCharacter(string("Lenard"), 10)); // The column is Lenard
   }
}
