incs += include_directories('.')

sources += 'Volts/GUI/GUI.cpp'

sources += 'Volts/GUI/GlobalFrame.cpp'

if host_machine.system() == 'darwin'
    # metal is only on mac
    sources += 'Volts/GUI/MacFrame.mm'
elif host_machine.system() == 'windows'
    sources += 'Volts/GUI/WindowsFrame.cpp'
else
    sources += 'Volts/GUI/LinuxFrame.cpp'
endif
