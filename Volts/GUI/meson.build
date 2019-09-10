dot = find_program('dotnet')

gui_target = custom_target('gui_target',
    output : 'VoltsGUI',
    command : [dot, 'build', '../Volts/GUI/', '-o', '@OUTPUT@']
)

gui = run_target('gui', command : gui_target)