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

args = []

# subprojects
# all in the subprojects/ folder
# decompression, used in UNSELF
zlib = subproject('zlib')
# fmt, used in logging
fmt = subproject('fmt')
# cthulhu, used for file IO
ctu = subproject('cthulhu')

# json, used for config stuff
args += '-DARGO_NOTHROW'
json = subproject('argo')
# tar, used for some ps3 files
tar = subproject('microtar')
# discord, used for custom discord presence
# TODO: should this be removed and moved to the gui frontends?
# TODO: probably lol
discord = subproject('discord')

# dependencies
# usually from pkg-config

threads = dependency('threads')

glfw_pack = dependency('glfw3', required : false)

deps = [
    ctu.get_variable('fs_dep'),
    ctu.get_variable('meta_dep'),
    ctu.get_variable('core_dep'),
    json.get_variable('argo_dep'),
    discord.get_variable('discord_dep'),
    tar.get_variable('tar_dep'),
    threads
]

if glfw_pack.found()
    message('found glfw3, ignoring git submodule')
    deps += glfw_pack
else
    message('glfw3 not found in system, using git submodule instead')
    deps += subproject('glfw').get_variable('glfw_dep')
endif

# get the very basic core sources
sources = []


if host_machine.system() == 'darwin'
    args += [
        # enable @import in objc++
        '-fmodules',
        # this fixes imgui metal backend
        '-fobjc-arc'
    ]
endif

incs = [
    include_directories('Volts'),
    ctu.get_variable('inc')
]

deps += [
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
    link_args : links,
    gui_app : true
)

