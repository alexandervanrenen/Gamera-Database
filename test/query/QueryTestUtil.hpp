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
   bool validateResult(unique_ptr<QueryResultCollection> result, const vector<vector<harriet::Value>>& expected)
   {
      valid = false;
      googleWrapper(move(result), expected);
      return valid;
   }
}
