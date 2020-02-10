project('elf', 'cpp',
    default_options : [
        'cpp_std=c++17'
    ]
)

svl = subproject('svl').get_variable('svl_dep')

elf_dep = declare_dependency(
    include_directories : include_directories('.'),
    dependencies : svl
)
