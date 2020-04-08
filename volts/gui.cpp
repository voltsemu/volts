#pragma once

#include <gac/gac.h>

GAC_MAIN({
    auto app = gac::app("volts", 1280, 720)
        -gac::nav({
            gac::item("Games", gac::icon::game())
                .destination(gac::text("games tab")),

            gac::item("View", gac::icon::view())
                .destination(gac::text("view tab")),

            gac::item("Files", gac::icon::files())
                .destination(gac::text("files tab")),
                
            gac::item("Tools", gac::icon::tools())
                .destination(gac::text("tools tab"))
        }).align(gac::align::top);

    app.run();
});
