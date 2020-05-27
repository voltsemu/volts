project('svl', 'cpp',
    default_options : [
        'cpp_std=c++17'
    ]
)

fmt = subproject('fmt').get_variable('fmt_dep')

libsvl = library('svl', [ 'svl/file.cpp' ],
    dependencies : [ fmt ]
)

svl_dep = declare_dependency(
    link_with : libsvl,
    dependencies : [ fmt ],
    include_directories : include_directories('.')
)