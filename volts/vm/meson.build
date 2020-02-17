libvm = library('vm', 'vm.cpp',
    dependencies : [ spdlog, svl ]
)

vm_inc = include_directories('.')

foreach module : ['ppu', 'spu', 'sys']
    subdir(module)
endforeach

