project('volts', 'cpp',
    default_options : [
        'cpp_std=c++17'
    ]
)

if not meson.is_subproject()

    cmd = subproject('cmd').get_variable('cmd_dep')

    cli = executable('volts', 'src/cli.cpp',
        dependencies : [ cmd ]
    )

endif