dot = find_program('dotnet')

gui_target = custom_target('gui_target',
    output : 'VoltsGUI',
    command : [dot, 'build', '../Volts/GUI/', '-o', meson.current_build_dir()],
    build_by_default : true
)
