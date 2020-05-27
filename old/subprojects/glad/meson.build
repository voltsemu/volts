project('glad', 'c')

inc = include_directories('include')

libglad = library('glad', 'src/glad.c',
    include_directories : inc
)

glad_dep = declare_dependency(
    link_with : libglad,
    include_directories : inc
)