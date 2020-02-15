vm = library('vm', 'vm.cpp',
    dependencies : [ spdlog, svl ]
)

foreach module : ['ppu', 'spu', 'sys']
    subdir(module)
endforeach

