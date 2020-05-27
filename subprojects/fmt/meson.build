project('fmt', 'cpp',
    default_options : [
        'cpp_std=c++14'
    ]
)

sources = [
    'fmt/src/format.cc',
    'fmt/src/os.cc'
]

inc = include_directories('fmt/include')

libfmt = library('fmt', sources,
    include_directories : inc
)

fmt_dep = declare_dependency(
    link_with : libfmt,
    include_directories : inc
)
