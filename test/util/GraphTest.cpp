// #include "test/TestConfig.hpp"
// #include "util/Utility.hpp"
// #include "common/Config.hpp"
// #include <gtest/gtest.h>
// #include "util/Graph.hpp"


// TEST(GraphTest, SimpleTest) {
//     dbi::Graph<uint64_t> g;
//     g.addEdge(1, 2);
//     g.addEdge(2, 3);
//     g.addEdge(1, 3);
//     g.addEdge(3, 4);
//     ASSERT_FALSE(g.hasCycle(1));
//     g.addEdge(4, 1);
//     ASSERT_TRUE(g.hasCycle(1));
//     g.clearNode(4);
//     ASSERT_FALSE(g.hasCycle(1));
// }
