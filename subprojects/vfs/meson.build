project('vfs', 'cpp',
    default_options : [
        'cpp_std=c++17'
    ]
)

svl = subproject('svl').get_variable('svl_dep')

libvfs = library('vfs', 'vfs.cpp',
    dependencies : svl
)

vfs_dep = declare_dependency(
    link_with : libvfs,
    include_directories : include_directories('.')
)