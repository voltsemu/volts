project('svl', 'cpp')

sources = [
    'svl/file.cpp',
    'svl/logger.cpp'
]

fmt = dependency('fmt', version : '>=6.1.0',
    fallback : [ 'fmt', 'fmt_dep' ]
)

incs = [ include_directories('.') ]

svl = library('svl', sources,
    include_directories : incs,
    dependencies : [ fmt ]
)

svl_dep = declare_dependency(
    include_directories : incs,
    dependencies : [ fmt ]
)
