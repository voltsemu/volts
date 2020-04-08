project('svl', 'cpp',
    default_options : [
        'cpp_std=c++17'
    ]
)

# gcc needs to link to stdc with fs support
if meson.get_compiler('cpp').get_id() == 'g++'
    args = '-lstdc++fs'
else
    args = []
endif

inc = include_directories('.')

spdlog_opts = [
    'enable_tests=false',
    'enable_examples=false',
    'no_exceptions=true',
    'no_thread_id=true',
    'no_tls=true'
]

if host_machine.system() == 'windows'
    spdlog_opts += 'wchar_support=true'
endif

spdlog = subproject('spdlog', default_options : spdlog_opts).get_variable('spdlog_dep')

libsvl = library('svl', [
        'svl/endian.cpp',
        'svl/file.cpp'
    ],
    include_directories : inc,
    install : true,
    cpp_args : args,
    dependencies : [ spdlog ]
)

svl_dep = declare_dependency(
    include_directories : inc,
    link_with : libsvl,
    dependencies : [ spdlog ]
)