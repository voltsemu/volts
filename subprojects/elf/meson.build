project('elf', 'cpp')

svl = subproject('svl').get_variable('svl_dep')

elf_dep = declare_dependency(
    dependencies : [ svl ],
    include_directories : include_directories('.')
)