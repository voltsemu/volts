#include <svl/cmd.h>

using namespace svl;

auto parser = cmd::cli("ps3 tools")({
    cmd::arg<fs::path>("sfo", "S", "path to a system file object to parse") = [](auto path) {

    },
    cmd::arg<fs::path>("output", "O", "redirect output, defaults to stdout") = [](auto path) {

    }
});

int main(int argc, const char** argv) {

}
