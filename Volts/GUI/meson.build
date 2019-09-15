warp = find_program('dotnet-warp')
dot = find_program('dotnet')

release_platform = ''
shared_ext = ''

if host_machine.system() == 'darwin'
    release_platform = 'osx-x64'
    shared_ext = 'dylib'
elif host_machine.system() == 'windows'
    release_platform = 'win-x64'
    shared_ext = 'dll'
elif host_machine.system() == 'linux'
    release_platform = 'linux-x64'
    shared_ext = 'so'
endif

copy_lib = custom_target('copy_lib',
    command : [python3, '-c', '''
import shutil, os
if not os.path.exists("@0@"):
    os.mkdir("@0@")
shutil.copyfile("@0@", "@1@.@2@")
    '''.format(vlib.full_path(), meson.current_build_dir()/'libvolt', shared_ext)],
    output : '.',
    depends : vlib
)

run_target('publish',
    command : [
        warp, meson.current_source_dir(),
        '-l', 'aggressive'
    ]
)

#compile the gui
run_target('gui',
    command : [
        dot, 'publish',
        meson.current_source_dir(),
        '-c', 'Release',
        '-r', release_platform,
        '-o', meson.current_build_dir(),
        '/p:TrimUnusedDependencies=true',
        '--self-contained', 'true'
    ],
    depends : copy_lib
)