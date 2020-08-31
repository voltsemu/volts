project('svl', 'cpp')

src = [ 'svl/file.cpp', 'svl/log.cpp' ]

inc = include_directories('.')

fmt = dependency('fmt', fallback : ['fmt', 'fmt_dep'])

svl = library('svl', src,
    include_directories : inc,
    dependencies : [fmt]
)

svl_dep = declare_dependency(
    link_with : svl,
    include_directories : inc,
    dependencies : [fmt]
)
