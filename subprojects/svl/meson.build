project('svl', 'cpp')

src = [ 'svl/file.cpp' ]

inc = include_directories('.')

svl = library('svl', src,
    include_directories : inc
)

svl_dep = declare_dependency(
    link_with : svl,
    include_directories : inc
)
