project('shaderc', [ 'c', 'cpp' ])

name = host_machine.system() + '-'

if get_option('buildtype').startswith('debug')
    name += 'debug'
else
    name += 'release'
endif

python = import('python3').find_python()

download = run_command(python, 'install.py', name, meson.current_source_dir())

if download.returncode() == 0
    shaderc_args = []
    shaderc_links = []
    if host_machine.system() == 'windows'
        shaderc_args += [ '/I' + meson.current_source_dir() / 'include' ]
        shaderc_links += [ '/LIBPATH:"' + meson.current_source_dir() / 'lib"' ]
    else
        shaderc_args += [ '-I' + meson.current_source_dir() / 'include' ]
        shaderc_links += [ '-L' + meson.current_source_dir() / 'lib/shaderc.a' ]
    endif
else
    error('failed to download shaderc source')
endif
