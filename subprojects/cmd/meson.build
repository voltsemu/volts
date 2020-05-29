project('cmd', 'cpp')

sources = [ ]

incs = [ include_directories('.') ]

fmt = dependency('fmt',
    version : '>=6.2.0',
    fallback : [ 'fmt', 'fmt_dep' ]
)

svl = subproject('svl').get_variable('svl_dep')

cmd_dep = declare_dependency(
    include_directories : incs,
    dependencies : [ fmt, svl ]
)