dot = find_program('dotnet')

#todo: find a way to stop this building everytime
custom_target('gui',
    output : 'VoltsGUI',
    command : [dot, 'build', meson.current_source_dir(), '-o', meson.current_build_dir()],
    build_by_default : true
)

release_platform = ''

if host_machine.system() == 'darwin'
    release_platform = 'osx-x64'
elif host_machine.system() == 'windows'
    release_platform = 'win-x64'
elif host_machine.system() == 'linux'
    release_platform = 'linux-x64'
endif

run_target('release', 
    command : [
        dot, 
        'publish', 
        meson.current_source_dir(), 
        '-c', 
        'Release', 
        '-r', 
        release_platform, 
        '-o', 
        meson.current_build_dir(),
        '--self-contained',
        'true',
        '/p:TrimUnusedDependencies=true'
    ],
    depends : vlib
)