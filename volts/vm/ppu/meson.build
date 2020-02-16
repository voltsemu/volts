ppu = library('ppu', [ 'module.cpp', 'thread.cpp' ],
    include_directories : vm_inc,
    link_with : [ vm ],
    dependencies : [ spdlog, svl, elf, xxhash, vfs ]
)