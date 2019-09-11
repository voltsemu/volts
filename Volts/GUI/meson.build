dot = find_program('dotnet')

release_platform = ''

if host_machine.system() == 'darwin'
    release_platform = 'osx-x64'
elif host_machine.system() == 'windows'
    release_platform = 'win-x64'
elif host_machine.system() == 'linux'
    release_platform = 'linux-x64'
endif

#compile the gui
compile_ui = custom_target('compile_gui', 
    command : [
        dot, 'publish', 
        meson.current_source_dir(), 
        '-c', 'Release', 
        '-r', release_platform, 
        '-o', meson.current_build_dir(),
        '--self-contained', 'true',
        '-v', 'q',
        '/p:TrimUnusedDependencies=true'
    ],
    depends : [
        vlib
    ],
    output : '.'
)

#then copy the shared library to the right place
run_target('release',
    command : ['cp', vlib.full_path(), meson.current_build_dir()],
    depends : compile_ui
)