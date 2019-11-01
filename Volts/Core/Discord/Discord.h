#pragma once

#include <string>

#include <Meta/Aliases.h>

namespace Volts::Discord
{
    using namespace Cthulhu;

    struct RPC
    {
        RPC();
        ~RPC();

        void SetGame(std::string Title);
    private:
        const char* ID = "639289003375591430";
        I64 StartTime = 0;
    };
}