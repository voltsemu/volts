project('volts', ['cpp', 'c'],
    default_options : [
        'cpp_std=c++17',
        'cpp_eh=none',
        'cpp_rtti=false'
    ],
    version : '0.2.0',
    license : 'Apache'
)

sources = []

log_opts = [
    'enable_examples=false',
    'enable_tests=false',
    'no_exceptions=true'
]

if host_machine.system() == 'windows'
    log_opts += 'wchar_support=true'
endif

logging = subproject('spdlog', 
    default_options : log_opts
)

json = subproject('rapidjson')

zlib = subproject('zlib',
    default_options : [
        'default_library=static'
    ]
)

deps = [ 
    logging.get_variable('spdlog_dep'), 
    json.get_variable('rapidjson_dep'),
    zlib.get_variable('zlib_dep')
]

incs = [ include_directories('volts') ]

links = []

args = []

subdir('volts')

# disable run time type id and exceptions in the hope of generating smaller code
# i know you dont pay for features you dont use but this also stops people
# from trying to use either feature at all so people dont """accidentally"""
# use these slow features for convenience sake
if meson.get_compiler('cpp').get_id() == 'msvc'
    args += ['/arch:AVX2', '/GF', '/wd4180'] #GF enables string pooling, we abuse this for performance reasons
    # wd4180 disables some pointless warnings that dont matter (yeah i know ignoring warnings is bad style)
else
    args += '-march=native'
endif

executable('volt', sources,
    include_directories : incs,
    install : true,
    cpp_args : args,
    dependencies : deps,
    link_args : links
)
