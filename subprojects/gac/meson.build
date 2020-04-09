project('gac', 'cpp',
    default_options : [
        'cpp_std=c++17'
    ]
)

sources = []

cpp_pch = ''


subdir('gac'/build_machine.system())

svl = subproject('svl').get_variable('svl_dep')

libgac = library('gac', sources,
    dependencies : svl,
    cpp_pch : cpp_pch
)

gac_dep = declare_dependency(
    link_with : [ libgac ],
    include_directories : include_directories('.')
)
