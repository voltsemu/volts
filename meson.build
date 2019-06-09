project('Volts', 'cpp',
    default_options : [ 'cpp_std=c++17' ],
    version : '0.2.0',
    license : 'Apache'
)

# get cthulhu for the standard library
cthulhu = subproject('Cthulhu')

# get zlib for decompressing files
zlib = subproject('zlib')

# get all the needed cthulhu projects
libs = [
    cthulhu.get_variable('metalib'),
    cthulhu.get_variable('corelib'),
    cthulhu.get_variable('fslib')
]

# get the very basic core sources
sources = [
    'Volts/Core/Logger/Logger.cpp',
    'Volts/Core/SIMD/SIMD.cpp',
    'Volts/Core/AES/AES.cpp',
    'Volts/Core/Files/TAR.cpp',
    'Volts/Volts.cpp',
]

incs = [
    include_directories('Volts'),
    include_directories('Volts/PS3'),
    cthulhu.get_variable('inc')
]

# include the ps3 project
subdir('Volts/PS3')

args = []

# disable run time type id and exceptions in the hope of generating smaller code
# i know you dont pay for features you dont use but this also stops people
# from trying to use either feature at all so people dont """accidentally"""
# use these slow features for convenience sake
if meson.get_compiler('cpp').get_id() == 'msvc'
    args += ['/GR-', '/arch:AVX2']
else
    args += ['-fno-rtti', '-fno-exceptions', '-march=native']
endif

if meson.get_compiler('cpp').get_id() == 'gcc'
    args += '-DGCC_WORKAROUND_BULLSHIT'
endif

deps = [zlib.get_variable('zlib_dep')]

if host_machine.system() == 'darwin'
    deps += dependency('appleframeworks', modules : 'coreservices')
endif

executable('volt', sources,
    include_directories : incs,
    link_with : libs,
    install : true,
    cpp_args : args,
    dependencies : deps
)
