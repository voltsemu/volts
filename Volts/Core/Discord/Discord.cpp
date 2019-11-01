#include "Discord.h"

#include <time.h>

#include "discord_rpc.h"

#include "Core/Generated/Version.h"

#include "Core/Emulator.h"

namespace Volts::Discord
{
    RPC::RPC()
    {
        SetGame("Nothing");
    }

    void RPC::SetGame(std::string Title)
    {
        StartTime = time(0);
        DiscordRichPresence Presence = {};
        Presence.state = fmt::format("Playing: {}", Title).c_str();
        Presence.details = fmt::format("Branch: {} Commit: {}", GIT_BRANCH, GIT_COMMIT).c_str();
        Presence.startTimestamp = StartTime;
        Presence.largeImageKey = "large-icon";
        Presence.smallImageKey = "small-icon";

        Discord_UpdatePresence(&Presence);
    }
}