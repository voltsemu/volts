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

# get zlib for decompressing files
zlib = subproject('zlib')

fmt = subproject('fmt')


# get the very basic core sources
sources = []

args = []

if host_machine.system() == 'darwin'
    args += [
        # enable @import in objc++
        '-fmodules',
        # this fixes imgui metal backend
        '-fobjc-arc',
        # this allows filesystem to work
        '-mmacosx-version-min=10.15'
    ]
endif

incs = [
    include_directories('Volts'),
    include_directories('Volts/PS3')
]

deps = [
    zlib.get_variable('zlib_dep'),
    fmt.get_variable('fmt_dep')
]

links = []

subdir('Volts')

# disable run time type id and exceptions in the hope of generating smaller code
# i know you dont pay for features you dont use but this also stops people
# from trying to use either feature at all so people dont """accidentally"""
# use these slow features for convenience sake
if meson.get_compiler('cpp').get_id() == 'msvc'
    args += ['/GR-', '/arch:AVX2', '/GF'] #GF enables string pooling, we abuse this for performance reasons
else
    args += ['-fno-rtti', '-fno-exceptions', '-march=native']
endif

if meson.get_compiler('cpp').get_id() == 'clang'
    args += [
        # meson doesnt set -std on clang
        '-std=c++17',
        # link to libc++ instead of default
        '-stdlib=libc++',
        # dont use default stdlib headers
        '-nostdinc++',
        # use these instead
        '-I/usr/local/opt/llvm/include/c++/v1'
    ]
    # also link to the new stdlib
    links += '-L/usr/local/opt/llvm/lib'
endif

executable('volt', sources,
    include_directories : incs,
    install : true,
    cpp_args : args,
    objcpp_args : args,
    dependencies : deps,
    link_args : links,
    gui_app : true
)

