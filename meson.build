project('volts', 'cpp',
    default_options : [
        'cpp_std=c++17',
        'cpp_eh=none',
        'cpp_rtti=false',

        'default_library=static'
    ]
)

svl = subproject('svl').get_variable('svl_dep')

fmt = dependency('fmt', version : '>=6.1.0',
    fallback : [ 'fmt', 'fmt_dep' ]
)

if not meson.is_subproject()

    cli = executable('vlt', 'src/cli.cpp',
        dependencies : [ svl, fmt ]
    )

    gui = executable('volts', 'src/gui.cpp',
        dependencies : [ svl, fmt ],
        gui_app : true
    )

endif