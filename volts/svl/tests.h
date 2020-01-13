#pragma once

#include <string_view>
#include <spdlog/spdlog.h>

#define CAT_INNER(x, y) x##y
#define CAT(x, y) CAT_INNER(x, y)

/* inline testing macros

to add a test simply write

TEST("test_name", {
    test_body;
    return test_value;
});

add the test will be automatically added to the test list
that can be run with run_tests()

*/
namespace svl::tests
{
    using test_func_t = bool(*)();
    struct test
    {
        test_func_t func;
        std::string_view name;
    };

#ifdef VTESTING
    void add_test(test func);
    void run_tests();
#   define ASSERT(...) if(!(__VA_ARGS__)) { spdlog::warn("ASSERT " #__VA_ARGS__ " failed"); return false; }
#   define TEST(name, ...) namespace { auto CAT(TEST, __COUNTER__) = []{ svl::tests::add_test(svl::tests::test{[]__VA_ARGS__, name}); return 0; }(); }
#else
#   define ASSERT(...)
#   define TEST(name, ...)
#endif

}