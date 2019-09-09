if host_machine.system() == 'windows'
    sources += [
        'Volts/PS3/GUI/Windows/GUI.cpp'
    ]
    links += 'WindowsApp.lib'
elif host_machine.system() == 'darwin'
    sources += [
        'Volts/PS3/GUI/Darwin/GUI.mm'
    ]
else
    sources += [
        'Volts/PS3/GUI/Linux/GUI.cpp'
    ]
endif