project('aes', 'c',
    license : 'free',
    default_options : [
        'c_std=c99'
    ]
)

source = 'aes.c'

include = include_directories('.')

libaes = library('aes', source,
    include_directories : include,
    install : true
)

aes_dep = declare_dependency(
    include_directories : include,
    link_with : libaes
)
