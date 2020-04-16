project('lmw', 'cpp')

sources = []

if host_machine.system() == 'windows'
    sources += 'lmw/lmwwindows.cpp'
endif

svl = subproject('svl').get_variable('svl_dep')

liblmw = library('lmw', sources,
    dependencies : [ svl ]
)

lmw_dep = declare_dependency(
    link_with : liblmw,
    include_directories : include_directories('.')
)