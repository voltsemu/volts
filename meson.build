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

volt_lib = static_library('VoltLib', sources,
    include_directories : [ inc, cthulhu.get_variable('inc') ],
    link_with : [
        cthulhu.get_variable('metalib'),
        cthulhu.get_variable('corelib'),
        cthulhu.get_variable('fslib')
    ],
    install : true
)

# put this beyond volt_lib decl so that meson does its thing
# turns out subdir('path') is basically #include "path"
if build_machine.system() == 'windows'
# building on windows
    subdir('Volts/Platform/Windows')
elif build_machine.system() == 'darwin'
# building on mac
    subdir('Volts/Platform/Apple')
else
# if its not mac or windows its going to be some linux flavor
    subdir('Volts/Platform/Linux')
endif
