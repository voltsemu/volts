project('volts', ['c', 'cpp'],
    default_options : [
        # keep to the latest common standard
        'cpp_std=c++17',

        # these features only cause headaches
        'cpp_rtti=false',
        'cpp_eh=none',

        'c_std=c11',

        # catch all the warnings we can
        'werror=true',
        'warning_level=3',

        # keep everything as flat as possible
        'default_library=static',
        'strip=true',

        # always be strict
        'b_lundef=true',

        # statically link the vsrt (issue 1)
        'b_vscrt=mt'
    ],
    version : '0.3.0',
    license : 'Apache 2.0'
)

# define _HAS_EXCEPTIONS=0 under msvc to prevent compiler errors (issue 2)
if meson.get_compiler('cpp').get_id() == 'msvc'
    add_global_arguments(['-D_HAS_EXCEPTIONS=0'], language : ['c', 'cpp'])
endif

svl = subproject('svl').get_variable('svl_dep')

deps = [ svl ]

sources = []

subdir('src/volts/loader')

libvolts = library('volts', sources,
    dependencies : deps
)

tomlplusplus = subproject('tomlplusplus').get_variable('tomlplusplus_dep')

cxxopts = subproject('cxxopts').get_variable('cxxopts_dep')

cli = executable('cli', 'src/cli.cpp',
    link_with : libvolts,
    dependencies : deps + [ cxxopts, tomlplusplus ]
)