project('cmd', 'cpp')

sources = [ ]

incs = [ include_directories('.') ]

fmt = dependency('fmt',
    version : '>=6.2.0',
    fallback : [ 'fmt', 'fmt_dep' ]
)

libcmd = library('cmd', sources,
    include_directories : incs,
    dependencies : [ fmt ]
)

cmd_dep = declare_dependency(
    link_with : libcmd,
    include_directories : incs,
    dependencies : [ fmt ]
)