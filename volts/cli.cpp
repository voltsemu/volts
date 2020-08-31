#include <svl/cmd.h>
#include <svl/log.h>

using namespace svl;

cmd::cli cli = cmd::cli("ps3 emulator tools", {
    cmd::str("output", "O", "redirect output to file", [](std::string str) {
        log::info("redirecting output to: {}", str);
        log::stream = std::make_unique<log::flog>(open(str, Mode::write));
    }),
    cmd::pos("help", "h", "print help message", [] {
        log::info(cli.help("vlt"));
    })
});

int main(int argc, const char** argv) {
    cli.parse(argc, argv);
}
