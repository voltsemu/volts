#include <cmd/cmd.h>
#include <fmt/core.h>

int main(int argc, char** argv)
{
    cmd::parser(argc, argv).with({
        cmd::flag("--help", "print help") += [](auto* parser) {
            fmt::print("volts\n{}", parser->help());
        }
    });
}