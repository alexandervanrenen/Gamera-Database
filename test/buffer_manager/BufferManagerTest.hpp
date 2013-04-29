#pragma once

#include "gtest/gtest.h"
#include "FunkesBufferManagerTest.hpp"
#include "buffer_manager/Config.hpp"
#include <array>
#include <fstream>

TEST(BufferManager, FunkeTest) {
    int argc = 5;
    std::array<const char*, 5> argv = {{"", "swap_file", "1024", "64", "2"}};

    std::ofstream out("swap_file");
    std::vector<char> data(dbi::kPageSize*1024);
    out.write(data.data(), dbi::kPageSize*1024);
    out.clear();

    ASSERT_EQ(main_funke(argc, const_cast<char**>(&std::get<0>(argv))),0);
}
