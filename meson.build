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
    add_global_arguments(['/D_HAS_EXCEPTIONS=0'], language : ['cpp', 'c'])
endif

fmt = dependency('fmt', fallback : ['fmt', 'fmt_dep'])
svl = subproject('svl').get_variable('svl_dep')
toml = subproject('toml').get_variable('tomlplusplus_dep')

deps = [svl, fmt, toml]

src = [
    'volts/loader/sfo.cpp',
    'volts/loader/pup.cpp'
]

args = ['-DTOML_EXCEPTIONS=0']

inc = include_directories(['volts/loader'])

cli = executable('vlt', src + [ 'volts/cli.cpp' ],
    include_directories : inc,
    dependencies : deps,
    cpp_args : args
)
