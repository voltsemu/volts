#pragma once

#include <gac/gac.h>

GAC_MAIN({
    auto app = gac::app("volts", 1280, 720)
        -gac::nav({
            gac::item("Games"),
            gac::item("Manage"),
            gac::item("View"),
            gac::item("Files")
        }).align(gac::align::top);

    app.run();
});