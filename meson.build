project('Volts', 'cpp', 
    default_options : [ 'cpp_std=c++17' ], 
    version : '0.2.0', 
    license : 'Apache'
)

cthulhu = subproject('Cthulhu')

zlib = subproject('zlib')

libs = [
    cthulhu.get_variable('metalib'),
    cthulhu.get_variable('corelib'),
    cthulhu.get_variable('fslib')
]

sources = [
    'Volts/Core/Logger/Logger.cpp',
    'Volts/Core/SIMD/SIMD.cpp',
    'Volts/Volts.cpp',
]

incs = [
    include_directories('Volts'),
    include_directories('Volts/PS3'),
    cthulhu.get_variable('inc')
]

subdir('Volts/PS3')

args = []

# disable run time type id and exceptions in the hope of generating smaller code
# i know you dont pay for features you dont use but this also stops people
# from trying to use either feature at all so people dont """accidentally""" 
# use these slow features for convenience sake
if meson.get_compiler('cpp').get_id() == 'msvc'
    args += ['/GR-', '/ZW', '/Zc:twoPhase-']
else
    args += ['-fno-rtti', '-fno-exceptions']
endif

if build_machine.system() == 'windows'
    static_library('volt', sources,
        include_directories : incs,
        link_with : libs,
        install : true,
        cpp_args : args,
        dependencies : zlib.get_variable('zlib_dep')
    )

    msproj = join_paths(meson.source_root(), 'Volts/Platform/Windows/Volts/Volts.sln')

    run_target('msbuild', 
        command : ['msbuild', msproj, '/p:Configuration=Release', '/p:Platform=x64', '/v:quiet']
    )

elif build_machine.system() == 'darwin'
    static_library('volt', sources,
        include_directories : incs,
        link_with : libs,
        install : true,
        cpp_args : args,
        dependencies : zlib.get_variable('zlib_dep')
    )

    xproj = join_paths(meson.source_root(), 'Volts/Platform/Darwin/Volts/Volts.xcodeproj')
    
    run_target('xcode',
        command : ['xcodebuild', '-project', xproj, '-scheme', 'Volts', '-configuration', 'Release', '-s']
    )

else
    executable('volt', sources,
        include_directories : incs,
        link_with : libs,
        install : true,
        cpp_args : args,
        dependencies : zlib.get_variable('zlib_dep')
    )
endif