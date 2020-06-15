project('crypt', 'cpp')

sources = [
    'crypt/aes.cpp'
]

svl = subproject('svl').get_variable('svl_dep')

crypt_dep = declare_dependency(
    include_directories : include_directories('.'),
    dependencies : [ svl ]
)