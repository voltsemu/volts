project('svl', 'cpp')

src = [ 'svl/file.cpp' ]

inc = include_directories('.')

fmt = dependency('fmt', fallback : ['fmt', 'fmt_dep'])

deps = [fmt]

svl = library('svl', src,
    include_directories : inc,
    dependencies : deps
)

svl_dep = declare_dependency(
    link_with : svl,
    include_directories : inc,
    dependencies : deps
)
