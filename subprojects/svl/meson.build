project('svl', 'cpp',
    default_options : [
        'cpp_std=c++17'
    ]
)

# gcc needs to link to stdc with fs support
if meson.get_compiler('cpp').get_id() == 'g++'
    args = '-lstdc++fs'
else
    args = []
endif

inc = include_directories('.')

libsvl = library('svl', [
        'endian.cpp',
        'file.cpp'
    ],
    include_directories : inc,
    install : true,
    cpp_args : args
)

svl_dep = declare_dependency(
    include_directories : inc,
    link_with : libsvl
)