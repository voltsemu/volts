project('Volts', 'cpp', 
    default_options : [ 'cpp_std=c++17' ], 
    version : '0.2.0', 
    license : 'Apache'
)

cthulhu = subproject('Cthulhu')

inc = include_directories('Volts')

libs = [
    cthulhu.get_variable('metalib'),
    cthulhu.get_variable('corelib'),
    cthulhu.get_variable('fslib')
]

subdir('Volts/PS3')

volts_sources = [
    'Volts/Core/Logger/Logger.cpp',
    'Volts/Volts.cpp'
]

executable('Volt', volts_sources,
    include_directories : [ inc, cthulhu.get_variable('inc') ],
    link_with : libs,
    install : true
)

