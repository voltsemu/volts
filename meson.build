project('RPCS3X', 'cpp', 
    default_options : [ 'cpp_std=c++17' ], 
    version : '0.2.0', 
    license : 'Apache'
)

cthulhu = subproject('Cthulhu')

inc = include_directories('RPCS3X')

sources = [
    'RPCS3X/Core/RPCS3X.cpp',
    'RPCS3X/Core/Utilities/SFO.cpp',
    'RPCS3X/Core/Utilities/Decrypt/UNSELF.cpp',
    'RPCS3X/Core/Utilities/Logger/Logger.cpp',
    'RPCS3X/Core/Render/Render.cpp',
    'RPCS3X/Core/Render/Null/Render.cpp'
]

executable('RPCS3X', sources,
    include_directories : [ inc, cthulhu.get_variable('inc') ],
    link_with : [
        cthulhu.get_variable('metalib'),
        cthulhu.get_variable('corelib'),
        cthulhu.get_variable('fslib')
    ],
    install : true
)