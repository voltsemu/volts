project('toml', 'cpp')

libtoml = library('toml', 'toml/toml.cpp',
    dependencies : subproject('svl').get_variable('svl_dep')
)

toml_dep = declare_dependency(
    link_with : libtoml,
    include_directories : include_directories('.')
)