project('Volts', ['cpp'],
    default_options : [
        'cpp_std=c++17',
        'cpp_eh=none'
    ],
    version : '0.2.0',
    license : 'Apache'
)

if host_machine.system() == 'darwin'
    add_languages('objcpp')
endif

# get cthulhu for the standard library
cthulhu = subproject('Cthulhu')

# get zlib for decompressing files
zlib = subproject('zlib')

fmt = subproject('fmt')

# get all the needed cthulhu projects
libs = [
    cthulhu.get_variable('metalib'),
    cthulhu.get_variable('corelib'),
    cthulhu.get_variable('fslib')
]

# get the very basic core sources
sources = [
    'Volts/Volts.cpp',
    'Volts/Config.cpp'
]

args = []

if host_machine.system() == 'darwin'
    # we need C++ modules to use objc @import in objcpp
    args += [
        '-fmodules',
        '-fobjc-arc'
    ]
endif

incs = [
    include_directories('Volts'),
    include_directories('Volts/PS3'),
    cthulhu.get_variable('inc')
]

deps = [
    zlib.get_variable('zlib_dep'),
    fmt.get_variable('fmt_dep')
]

links = []

# include the ps3 project
subdir('Volts/PS3')
subdir('Volts/GUI')
subdir('Volts/Core')
subdir('Volts/External')

# disable run time type id and exceptions in the hope of generating smaller code
# i know you dont pay for features you dont use but this also stops people
# from trying to use either feature at all so people dont """accidentally"""
# use these slow features for convenience sake
if meson.get_compiler('cpp').get_id() == 'msvc'
    args += ['/GR-', '/arch:AVX2', '/GF'] #GF enables string pooling, we abuse this for performance reasons
else
    args += ['-fno-rtti', '-fno-exceptions', '-march=native']
endif

if meson.get_compiler('cpp').get_id() == 'gcc'
    args += '-DGCC_WORKAROUND_BULLSHIT'
endif

if meson.get_compiler('cpp').get_id() == 'clang'
    args += '-std=c++17'
endif

executable('volt', sources,
    include_directories : incs,
    link_with : libs,
    install : true,
    cpp_args : args,
    objcpp_args : args,
    dependencies : deps,
    link_args : links,
    gui_app : true
)

