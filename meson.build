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

glfw_pack = dependency('glfw3', required : false)

deps = []

if glfw_pack.found()
    message('found glfw3, ignoring git submodule')
    deps += glfw_pack
else
    message('glfw3 not found in system, using git submodule instead')
    deps += subproject('glfw').get_variable('glfw_dep')
endif

# get the very basic core sources
sources = []

args = []

if host_machine.system() == 'darwin'
    args += [
        # enable @import in objc++
        '-fmodules',
        # this fixes imgui metal backend
        '-fobjc-arc'
    ]
endif

incs = [
    include_directories('Volts')
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
    args += ['/GR-', '/arch:AVX2', '/GF'] #GF enables string pooling, we abuse this for performance reasons
else
    args += ['-fno-rtti', '-fno-exceptions', '-march=native']
endif

if meson.get_compiler('cpp').get_id() == 'clang'
    args += '-std=c++17'
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

