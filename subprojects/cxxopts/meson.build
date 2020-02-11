project('cxxopts', 'cpp',
    default_options : [
        'cpp_std=c++11'
    ]
)

libcxxopts = library('cxxopts', 'cxxopts/include/cxxopts.hpp')

cxxopts_dep = declare_dependency(
    link_with : libcxxopts,
    include_directories : include_directories('cxxopts/include')
)