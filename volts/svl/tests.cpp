#include "tests.h"
#include "types.h"

#include <vector>
#include <atomic>
#include <algorithm>
#include <execution>
#include <spdlog/spdlog.h>

namespace svl::tests
{
    using namespace svl;
    std::vector<test>& get_tests()
    {
        static std::vector<test> tests = {};
        return tests;
    }

    void add_test(test func)
    {
        get_tests().push_back(func);
    }

    void run_tests()
    {
        spdlog::info("running {} tests", get_tests().size());

        std::atomic<u64> failed_tests = 0;
        std::for_each(
            std::execution::par_unseq, 
            get_tests().begin(), 
            get_tests().end(),
            [&failed_tests](auto&& test) {
                if(!test.func())
                {
                    failed_tests.fetch_add(1, std::memory_order_relaxed);
                    spdlog::warn("test {} failed", test.name);
                }
                else
                {
                    spdlog::info("test {} passed", test.name);
                }
            }
        );

        if(failed_tests)
        {
            spdlog::error("{} tests have failed", failed_tests);
        }

        spdlog::info("finished tests");
    }
}
