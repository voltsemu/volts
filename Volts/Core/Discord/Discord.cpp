#include "Discord.h"

#include <time.h>

#include "discord_rpc.h"

#include "Core/Generated/Version.h"

#include "Core/Emulator.h"

namespace Volts::Discord
{
    RPC::RPC()
    {
        Discord_Initialize(ID, nullptr, 1, nullptr);
        SetGame("Nothing");
    }

    RPC::~RPC()
    {
        Discord_Shutdown();
    }

    void RPC::SetGame(std::string Title)
    {
        State = fmt::format("{}:{} Playing: {}", GIT_BRANCH, GIT_COMMIT, Title);
        DiscordRichPresence Presence = {};
        Presence.state = State.c_str();
        Presence.startTimestamp = time(0);
        Presence.largeImageKey = "shrek_0";
        Presence.smallImageKey = "mynameehjeff_1_";

        Discord_UpdatePresence(&Presence);
    }
}