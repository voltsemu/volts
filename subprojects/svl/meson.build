project('svl', 'cpp',
    default_options : [
        'cpp_std=c++17'
    ]
)

fmt = subproject('fmt').get_variable('fmt_dep')

sources = [
    'svl/file.cpp',
    'svl/log.cpp'
]

libsvl = library('svl', sources,
    dependencies : [ fmt ]
)

svl_dep = declare_dependency(
    link_with : libsvl,
    dependencies : [ fmt ],
    include_directories : include_directories('.')
)