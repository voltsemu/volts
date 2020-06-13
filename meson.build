project('volts', 'cpp',
    default_options : [
        'cpp_std=c++17',
        'cpp_eh=none',
        'cpp_rtti=false',

        'warning_level=3',
        'werror=true',

        'default_library=static',
        'b_vscrt=mt'
    ],
    license : 'Apache 2.0',
    version : '0.0.1'
)

if meson.get_compiler('cpp').get_id() == 'msvc'
    add_global_arguments(['/D_HAS_EXCEPTIONS=0'], language : [ 'c', 'cpp' ])
endif

svl = subproject('svl').get_variable('svl_dep')

elf = subproject('elf').get_variable('elf_dep')

crypt = subproject('crypt').get_variable('crypt_dep')

sources = [
    'volts/loader/self.cpp',
    'volts/loader/keys/keys.cpp'
]



libvolts = library('volts', sources,
    include_directories : include_directories('.'),
    dependencies : [ svl, elf, crypt ]
)

volts_dep = declare_dependency(
    link_with : libvolts,
    include_directories : include_directories('.'),
    dependencies : [ svl, elf, crypt ]
)

if not meson.is_subproject()

    cli = executable('vlt', 'volts/cli.cpp',
        dependencies : [ volts_dep ]
    )

    gui = executable('volt', 'volts/gui.cpp',
        dependencies : [ volts_dep ],
        gui_app : true
    )

endif