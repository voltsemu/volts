project('volts', 'cpp',
    default_options : [
        'cpp_std=c++17',
        'cpp_rtti=false',
        'cpp_eh=none',
        # as the VC 2019 runtime isnt shipped by default on windows we need to link it statically
        # this stops those weird dll errors that lead to downloading some random dll
        # and shoving it into system32
        'b_vscrt=mt',
        'default_library=static',
        'werror=true',

        # TODO: we will need to replace xxhash and zlib at some point
        # as they trigger all kinds of warnings at higher levels
        'warning_level=3'
    ],
    version : '0.2.2',
    license : 'Apache'
)

# TODO: using cpp_eh=none makes meson do funky stuff on windows, feels like a bug, should report
if meson.get_compiler('cpp').get_id() == 'msvc'
    add_global_arguments(['/D_HAS_EXCEPTIONS=0'],
        language : ['cpp', 'c']
    )
endif

svl = subproject('svl').get_variable('svl_dep')

deps = [ svl ]
src = []
inc = include_directories()

cli = executable('vlt', src + [ 'volts/cli.cpp' ],
    include_directories : inc,
    dependencies : deps
)
