project('Volts', ['cpp'],
    default_options : [ 'cpp_std=c++17' ],
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
    'Volts/Core/IO/File.cpp',
    'Volts/Volts.cpp',
]

args = []

if host_machine.system() == 'darwin'
    sources += 'Volts/Core/Darwin/Darwin.mm'
    # we need C++ modules to use objc @import in objcpp
    args += '-fmodules'
endif

incs = [
    include_directories('Volts'),
    include_directories('Volts/PS3'),
    cthulhu.get_variable('inc')
]

deps = [zlib.get_variable('zlib_dep')]
links = []

pymodule = import('python3')
python3 = pymodule.find_python()

vulkan = false

if host_machine.system() == 'darwin'
    deps += dependency('appleframeworks', modules : ['Foundation', 'Metal', 'Cocoa'])
else
    # apple doesnt have support for vulkan

    #check if we have the vulkan sdk
    res = run_command(python3, '-c', 'import os; print(os.environ["VULKAN_SDK"])')

    # returns non-zero because an exception is thrown if the sdk isnt found
    if res.returncode() != 0
        message('Vulkan SDK not found, not building with vulkan')
    else
        vulkan = true
        # if we do then we should use it
        incs += res.stdout().strip() / 'Include'
        links += res.stdout().strip() / 'Lib' / 'vulkan-1.lib'
    endif
endif

# include the ps3 project
subdir('Volts/PS3')

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

if meson.get_compiler('cpp').get_id() == 'clang'
    args += '-std=c++17'
endif

if host_machine.system() == 'windows'
    links += '/SUBSYSTEM:WINDOWS'
endif

vlib = shared_library('volt', sources,
    include_directories : incs,
    link_with : libs,
    install : true,
    cpp_args : args,
    objcpp_args : args,
    dependencies : deps,
    link_args : links
)

subdir('Volts/GUI')
