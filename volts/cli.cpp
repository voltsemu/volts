#include <svl/cmd.h>

using namespace svl;

cmd::cli cli = cmd::cli("ps3 emulator tools", {
    cmd::str("output", "O", "redirect output to file", [](std::string str) {
        fmt::print("redirecting output to: {}\n", str);
    }),
    cmd::pos("help", "h", "print help message", [] {
        fmt::print("{}\n", cli.help("vlt"));
        std::exit(0);
    })
});

int main(int argc, const char** argv) {
    cli.parse(argc, argv);
}
