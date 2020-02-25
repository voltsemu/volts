project('shaderc', [ 'c', 'cpp' ])

if get_option('buildtype').startswith('debug')
    if host_machine.system() == 'windows'
        url = 'https://storage.googleapis.com/shaderc/badges/build_link_windows_vs2017_debug.html'
    elif host_machine.system() == 'darwin'
        url = 'https://storage.googleapis.com/shaderc/badges/build_link_macos_clang_debug.html'
    else
        #todo: match to current compiler
        url = 'https://storage.googleapis.com/shaderc/badges/build_link_linux_clang_debug.html'
    endif
else
    if host_machine.system() == 'windows'
        url = 'https://storage.googleapis.com/shaderc/badges/build_link_windows_vs2017_release.html'
    elif host_machine.system() == 'darwin'
        url = 'https://storage.googleapis.com/shaderc/badges/build_link_macos_clang_release.html'
    else
        # todo: match to current compiler on linux
        url = 'https://storage.googleapis.com/shaderc/badges/build_link_linux_clang_release.html'
    endif
endif

curl = find_program('curl')

download = custom_target('download',
    input : url,
    output : 'install.zip',
    command : [ curl, '@INPUT@', '--output', '@OUTPUT@' ]
)

if host_machine.system() == 'windows'
    extract = run_command('extract.vbs', download, meson.current_source_dir())
else
    unzip = find_program('unzip')

    extract = custom_target('extract',
        input : download,
        output : 'shaderc',
        command : [ unzip, '@INPUT@', '-d', '@OUTPUT@' ]
    )
endif

libshaderc = static_library('shaderc', extract)

shaderc_dep = declare_dependency(
    link_with : libshaderc
)