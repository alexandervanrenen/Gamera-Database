#include "gtest/gtest.h"
#include "core/Database.hpp"
#include "core/DatabaseConfig.hpp"
#include "test/TestConfig.hpp"
#include "query/result/QueryResult.hpp"
#include "query/result/QueryResultCollection.hpp"
#include "harriet/Value.hpp"
#include "util/Utility.hpp"
#include <set>

using namespace std;
using namespace dbi;

namespace {
   bool valid;
   void googleWrapper(unique_ptr<QueryResultCollection> result, const vector<vector<harriet::Value>>& expected)
   {
      ASSERT_TRUE(result->getAsSelect() != nullptr);
      auto& selectResult = *result->getAsSelect();
      ASSERT_TRUE(selectResult.result.size() == expected.size());
      for(uint32_t row=0; row<expected.size(); row++) {
         ASSERT_TRUE(selectResult.result[row].size() == expected[row].size());
         for(uint32_t column=0; column<expected[row].size(); column++)
            ASSERT_EQ(selectResult.result[row][column], expected[row][column]);
      }
      valid = true;
   }
   bool validateSelection(unique_ptr<QueryResultCollection> result, const vector<vector<harriet::Value>>& expected)
   {
      valid = false;
      googleWrapper(move(result), expected);
      return valid;
   }
}

TEST(Query, GoodRoundTrip)
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
   ASSERT_TRUE(validateSelection(move(queryResultCollection), result));
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
      vector<vector<harriet::Value>> result(1);
      result[0].emplace_back(harriet::Value::createCharacter(string("Lenard"), 10));
      ASSERT_TRUE(validateSelection(move(queryResultCollection), result));
   }

   // Restart database
   {
      Database db(config, false);
      auto queryResultCollection = db.executeQuery("select name from Persons;");

      // Check query
      vector<vector<harriet::Value>> result(1);
      result[0].emplace_back(harriet::Value::createCharacter(string("Lenard"), 10));
      ASSERT_TRUE(validateSelection(move(queryResultCollection), result));
   }
}

TEST(Query, Selection)
{
   DatabaseConfig config{kSwapFileName, kSwapFilePages};

   // Start database
   Database db(config, true);
   auto query = util::loadFileToMemory("sql/schema/big_bang_theory.sql");
   ASSERT_TRUE(db.executeQuery(query)->good());

   vector<vector<harriet::Value>> expectedLen(1);
   expectedLen[0].emplace_back(harriet::Value::createCharacter(string("Lenard"), 10));

   vector<vector<harriet::Value>> expectedAll(5);
   expectedAll[0].emplace_back(harriet::Value::createCharacter(string("Lenard"), 10));
   expectedAll[1].emplace_back(harriet::Value::createCharacter(string("Penny"), 10));
   expectedAll[2].emplace_back(harriet::Value::createCharacter(string("Sheldon"), 10));
   expectedAll[3].emplace_back(harriet::Value::createCharacter(string("Rajesh"), 10));
   expectedAll[4].emplace_back(harriet::Value::createCharacter(string("Howard"), 10));

   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where firstname = 'Lenard';"), expectedLen));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where Persons.firstname = 'Lenard';"), expectedLen));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where 'Lenard' = firstname;"), expectedLen));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where 'Lenard' = Persons.firstname;"), expectedLen));

   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and firstname = 'Lenard';"), expectedLen));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and lastname = 'Hofstadter';"), expectedLen));

   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and 2 = 2;"), expectedLen));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where 2 = 2 and firstname = 'Lenard';"), expectedLen));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and 2 = 3;"), vector<vector<harriet::Value>>(0)));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where 2 = 3 and firstname = 'Lenard';"), vector<vector<harriet::Value>>(0)));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where false;"), vector<vector<harriet::Value>>(0)));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where false = true;"), vector<vector<harriet::Value>>(0)));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where true = true;"), expectedAll));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where true;"), expectedAll));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons;"), expectedAll));

   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and (id+1)*2*2 = 4;"), expectedLen));
   ASSERT_TRUE(validateSelection(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and id=id;"), expectedLen));
}
