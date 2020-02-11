project('alert', 'cpp',
    default_options : [
        'cpp_std=c++98'
    ]
)

if host_machine.system() == 'darwin'
    add_languages('objcpp')
    source = 'alert.mm'
else
    source = 'alert.cpp'
endif

include = include_directories('.')

libalert = library('alert', source,
    include_directories : include,
    install : true
)

alert_dep = declare_dependency(
    include_directories : include,
    link_with : libalert
)
