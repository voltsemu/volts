foreach dir : [ 'crypt', 'loader', 'gui' ]
    subdir(dir)
endforeach

cli = executable('cli', [
        'cli.cpp'
    ],
    link_with : [ libload, libcrypt ],
    dependencies : [ spdlog, svl, elf, vfs, cxxopts, rapidjson ]
)

gui = executable('gui', [ 'gui.cpp' ],
    link_with : [ libgui ],
    dependencies : [ svl, vfs ],
    gui_app : true
)