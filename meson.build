project('volts', ['cpp'],
    default_options : [
        'cpp_std=c++17',
        'cpp_eh=none'
    ],
    version : '0.2.0',
    license : 'Apache'
)

sources = []

logging = subproject('spdlog', 
    default_options : [
        'enable_examples=false',
        'enable_tests=false',
        'no_exceptions=true'
    ]
)

deps = [ logging.get_variable('spdlog_dep') ]

incs = [ include_directories('volts') ]

links = []

args = []

subdir('volts')

# disable run time type id and exceptions in the hope of generating smaller code
# i know you dont pay for features you dont use but this also stops people
# from trying to use either feature at all so people dont """accidentally"""
# use these slow features for convenience sake
if meson.get_compiler('cpp').get_id() == 'msvc'
    args += ['/GR-', '/arch:AVX2', '/GF', '/EHsc-'] #GF enables string pooling, we abuse this for performance reasons
else
    args += ['-fno-rtti', '-fno-exceptions', '-march=native']
endif

if meson.get_compiler('cpp').get_id() == 'clang'
    args += '-std=c++17'
endif

if meson.get_compiler('cpp').get_id() == 'gcc'
    args += '-DGCC_WORKAROUND_BULLSHIT'
endif

executable('volt', sources,
    include_directories : incs,
    install : true,
    cpp_args : args,
    objcpp_args : args,
    dependencies : deps,
    link_args : links
)

