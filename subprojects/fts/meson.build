project('fuzzy', 'cpp',
    license : 'public',
    default_options : [
        'c_std=c99'
    ]
)

source = 'fuzzy_match.cpp'

include = include_directories('.')

libfuzzy = library('fuzzy', source,
    include_directories : include,
    install : true
)

fuzzy_dep = declare_dependency(
    include_directories : include,
    link_with : libfuzzy
)
