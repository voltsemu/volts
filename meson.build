project('volts', 'cpp',
    default_options : [
        'cpp_std=c++17',
        'cpp_eh=none',
        'cpp_rtti=false',

        'default_library=static'
    ]
)

if not meson.is_subproject()

    cmd = subproject('cmd').get_variable('cmd_dep')

    cli = executable('volts', 'src/cli.cpp',
        dependencies : [ cmd ]
    )

endif