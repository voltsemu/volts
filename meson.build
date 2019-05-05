project('Volts', 'cpp', 
    default_options : [ 'cpp_std=c++17' ], 
    version : '0.2.0', 
    license : 'Apache'
)

cthulhu = subproject('Cthulhu')

inc = include_directories('Volts')

sources = [
    'Volts/Core/Volts.cpp',
    'Volts/Core/Utilities/SFO.cpp',
    'Volts/Core/Utilities/Decrypt/UNSELF.cpp',
    'Volts/Core/Utilities/Logger/Logger.cpp',
    'Volts/Core/Utilities/Decrypt/Keys.cpp',
    'Volts/Core/Render/Render.cpp',
    'Volts/Core/Render/Null/Render.cpp'
]

if build_machine.system() == 'windows'
    # just so much funky windows stuff to do

    executable('Volt', sources,
        include_directories : [ inc, cthulhu.get_variable('inc') ],
        link_with : [
            cthulhu.get_variable('metalib'),
            cthulhu.get_variable('corelib'),
            cthulhu.get_variable('fslib')
        ],
        install : true
    )

else
    # everywhere else this stuff can be statically linked
    executable('Volt', sources,
        include_directories : [ inc, cthulhu.get_variable('inc') ],
        link_with : [
            cthulhu.get_variable('metalib'),
            cthulhu.get_variable('corelib'),
            cthulhu.get_variable('fslib')
        ],
        install : true
    )
endif
