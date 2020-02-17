libppu = library('ppu', [ 'module.cpp', 'thread.cpp' ],
    include_directories : vm_inc,
    link_with : [ libvm ],
    dependencies : [ spdlog, svl, elf, xxhash, vfs ]
)