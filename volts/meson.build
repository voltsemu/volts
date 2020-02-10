foreach dir : [ 'crypt', 'loader', 'vfs' ]
    subdir(dir)
endforeach

cli = executable('cli', [
        'cmd.cpp'
    ],
    link_with : [ libload, libcrypt, libvfs ],
    dependencies : [ spdlog, svl, elf, cxxopts, rapidjson ]
)