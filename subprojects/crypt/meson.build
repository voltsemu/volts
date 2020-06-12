project('crypt', 'cpp')

sources = [
    'crypt/aes.cpp'
]

svl = subproject('svl').get_variable('svl_dep')

libcrypt = library('crypt', sources,
    dependencies : [ svl ]
)

crypt_dep = declare_dependency(
    link_with : libcrypt,
    include_directories : include_directories('.'),
    dependencies : [ svl ]
)