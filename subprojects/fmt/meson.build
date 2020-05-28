project('fmt', 'cpp',
    version : '6.2.1'
)

sources = [ 'fmt/src/format.cc', 'fmt/src/os.cc' ]

incs = [ include_directories('fmt/include') ]

libfmt = library('fmt', sources,
    include_directories : incs
)

fmt_dep = declare_dependency(
    link_with : libfmt,
    include_directories : incs
)
