project('crypt', 'cpp')

sources = [
    'crypt/aes.cpp'
]

svl = subproject('svl').get_variable('svl_dep')

if get_option('static_rom').enabled()
    args = [ '-DROM_TABLES=1' ]
else
    args = []
endif

libcrypt = library('crypt', sources,
    cpp_args : args,
    dependencies : [ svl ]
)

crypt_dep = declare_dependency(
    link_with : libcrypt,
    include_directories : include_directories('.'),
    dependencies : [ svl ]
)