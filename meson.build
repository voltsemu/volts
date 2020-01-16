project('volts', ['cpp', 'c'],
    default_options : [
        'cpp_std=c++17',
        'cpp_rtti=false',
        'cpp_eh=none',
        'default_library=static'
    ],
    version : '0.2.0',
    license : 'Apache'
)

#TODO: using cpp_eh=none makes meson do funky stuff on windows, feels like a bug, should report
if meson.get_compiler('cpp').get_id() == 'msvc'
    add_global_arguments('/D_HAS_EXCEPTIONS=0', language : 'cpp')
endif

sources = []
test_sources = []

log_opts = [
    'enable_tests=false',
    'enable_examples=false',
    'no_exceptions=true',
    'no_thread_id=true',
    'no_tls=true'
]

if host_machine.system() == 'windows'
    log_opts += 'wchar_support=true'
endif

logging = subproject('spdlog', 
    default_options : log_opts
)

json = subproject('rapidjson')

zlib = dependency('zlib', required : false)

if not zlib.found()
    zlib = subproject('zlib').get_variable('zlib_dep')
endif

hash = subproject('xxhash').get_variable('xxhash_dep')

deps = [ 
    logging.get_variable('spdlog_dep'), 
    json.get_variable('rapidjson_dep'),
    zlib,
    hash
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
    args += ['/GF', '/wd4180'] #GF enables string pooling, we abuse this for performance reasons
    # wd4180 disables some pointless warnings that dont matter (yeah i know ignoring warnings is bad style)
endif

executable('volt', sources,
    include_directories : incs,
    install : true,
    cpp_args : args,
    dependencies : deps,
    link_args : links
)

executable('volt_tests', sources + test_sources,
    include_directories : incs,
    install : false,
    cpp_args : args + [ '-DVTESTING=1' ],
    dependencies : deps,
    link_args : links,
    build_by_default : false
)
