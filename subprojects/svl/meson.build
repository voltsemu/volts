project('svl', 'cpp')

sources = [
    'svl/file.cpp'
]

incs = [ include_directories('.') ]

svl = library('svl', sources,
    include_directories : incs
)

svl_dep = declare_dependency(
    include_directories : incs
)