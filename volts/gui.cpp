#pragma once

#include <gac/app.h>

GAC_MAIN({
    auto app = gac::app("volts", 1280, 720);

    app.run();
});