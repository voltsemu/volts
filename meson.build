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
    cthulhu.get_variable('fslib'),
    zlib.get_variable('zlib')
]

sources = [
    'Volts/Core/Logger/Logger.cpp',
    'Volts/Core/SIMD/SIMD.cpp',
    'Volts/Volts.cpp',
]

incs = [
    include_directories('Volts'),
    include_directories('Volts/PS3'),
    cthulhu.get_variable('inc'),
    zlib.get_variable('incdir')
]

subdir('Volts/PS3')

executable('volt', sources,
    include_directories : incs,
    link_with : libs,
    install : true
)

