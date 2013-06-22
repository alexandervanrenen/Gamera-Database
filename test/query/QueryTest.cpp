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

struct QueryTest : public testing::Test {

  virtual void SetUp() {
     // Start database
     DatabaseConfig config{kSwapFileName, kSwapFilePages};
     database = util::make_unique<Database>(config, true);
     auto query = util::loadFileToMemory("sql/schema/big_bang_theory.sql");
     ASSERT_TRUE(database->executeQuery(query)->good());

     // Prepare results
     expectedLen.resize(1);
     expectedLen[0].emplace_back(harriet::Value::createCharacter(string("Lenard"), 10));

     expectedAll.resize(5);
     expectedAll[0].emplace_back(lenard.createCopy());
     expectedAll[1].emplace_back(harriet::Value::createCharacter(string("Penny"), 10));
     expectedAll[2].emplace_back(harriet::Value::createCharacter(string("Sheldon"), 10));
     expectedAll[3].emplace_back(harriet::Value::createCharacter(string("Rajesh"), 10));
     expectedAll[4].emplace_back(harriet::Value::createCharacter(string("Howard"), 10));
  }

  harriet::Value lenard = harriet::Value::createCharacter(string("Lenard"), 10);

  vector<vector<harriet::Value>> expectedLen;
  vector<vector<harriet::Value>> expectedAll;
  vector<vector<harriet::Value>> expectedNone;
  unique_ptr<Database> database;
};

TEST_F(QueryTest, SelectionGood)
{
   auto& db = *database;

   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where firstname = 'Lenard';"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where Persons.firstname = 'Lenard';"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where 'Lenard' = firstname;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where 'Lenard' = Persons.firstname;"), expectedLen));

   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and firstname = 'Lenard';"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and lastname = 'Hofstadter';"), expectedLen));

   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and 2 = 2;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where 2 = 2 and firstname = 'Lenard';"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and 2 = 3;"), expectedNone));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where 2 = 3 and firstname = 'Lenard';"), expectedNone));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where false;"), expectedNone));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where false = true;"), expectedNone));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where true = true;"), expectedAll));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where true;"), expectedAll));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where true = false = false;"), expectedAll));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where true = true = false;"), expectedNone));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons;"), expectedAll));

   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and (id+1)*2*2 = 4;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where firstname = 'Lenard' and id=id;"), expectedLen));
}

TEST_F(QueryTest, ProjectionGood)
{
   auto& db = *database;

   vector<vector<harriet::Value>> expectedTwo(1);
   expectedTwo[0].emplace_back(lenard.createCopy());
   expectedTwo[0].emplace_back(lenard.createCopy());

   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons where id = 0;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select Persons.firstname from Persons where id = 0;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select p.firstname from Persons p where id = 0;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname from Persons p where id = 0;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select firstname, firstname from Persons where id = 0;"), expectedTwo));
   ASSERT_TRUE(validateResult(db.executeQuery("select p.firstname, firstname from Persons p where id = 0;"), expectedTwo));
}

TEST_F(QueryTest, JoinsGood)
{
   auto& db = *database;

   ASSERT_TRUE(validateResult(db.executeQuery("select p1.firstname from Persons p1, Persons p2 where p1.id = p2.id;"), expectedAll));
   ASSERT_TRUE(validateResult(db.executeQuery("select p1.firstname from Persons p1, Persons p2 where p1.id = p2.id and p1.id = 0;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select p1.firstname from Persons p1, Persons p2 where p1.id = p2.id and p2.id = 0;"), expectedLen));
   ASSERT_TRUE(validateResult(db.executeQuery("select p2.firstname from Persons p1, Relations r, Persons p2 where p1.firstname = 'Penny' and r.type = 'sex' and r.lhs = p1.id and r.rhs = p2.id;"), expectedLen));
}
