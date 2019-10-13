sources += 'Volts/Core/Frame/Window.cpp'

if host_machine.system() == 'darwin'
    sources += 'Volts/Core/Frame/DarwinWindow.mm'
    deps += dependency('appleframeworks', modules : [ 'Cocoa', 'Foundation' ])
elif host_machine.system() == 'linux'
    sources += 'Volts/Core/Frame/LinuxWindow.cpp'
elif host_machine.system() == 'windows'
    sources += 'Volts/Core/Frame/WindowsWindow.cpp'
endif