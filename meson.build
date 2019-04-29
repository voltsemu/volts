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
    'Volts/Core/Render/Render.cpp',
    'Volts/Core/Render/Null/Render.cpp'
]

if build_machine.system() == 'windows'
    # just so much funky windows stuff to do
    # since we make a UWP app on windows this needs to be shared

    volt_lib = shared_library('VoltLib', sources,
        cpp_args : ['/ZW', '/EHsc', '/Zc:twoPhase-'],
        name_prefix : '',
        name_suffix : 'dll',
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
    volt_lib = static_library('VoltLib', sources,
        include_directories : [ inc, cthulhu.get_variable('inc') ],
        link_with : [
            cthulhu.get_variable('metalib'),
            cthulhu.get_variable('corelib'),
            cthulhu.get_variable('fslib')
        ],
        install : true
    )
endif

# put this beyond volt_lib decl so that meson does its thing
# turns out subdir('path') is basically #include "path"
if build_machine.system() == 'darwin'
# building on mac
    subdir('Volts/Platform/Apple')
elif build_machine.system() == 'windows' # stubbed
else
# if its not mac or windows its going to be some linux flavor
    subdir('Volts/Platform/Linux')
endif
