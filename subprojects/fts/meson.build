project('fuzzy', 'cpp',
    license : 'public',
    default_options : [
        'c_std=c99'
    ]
)

source = 'fuzzy_match.cpp'

include = include_directories('.')

libaes = library('fuzzy', source,
    include_directories : include,
    install : true
)

aes_dep = declare_dependency(
    include_directories : include,
    link_with : libaes
)
