project('volts', 'cpp',
    default_options : [
        'cpp_std=c++17',
        'cpp_eh=none',
        'cpp_rtti=false',

        'warning_level=3',
        'werror=true',

        'default_library=static'
    ]
)

svl = subproject('svl').get_variable('svl_dep')

if not meson.is_subproject()

    cli = executable('vlt', 'src/cli.cpp',
        dependencies : [ svl ]
    )

    gui = executable('volts', 'src/gui.cpp',
        dependencies : [ svl ],
        gui_app : true
    )

endif