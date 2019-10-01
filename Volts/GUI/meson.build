subdir('imgui')

incs += include_directories('.')

sources += 'Volts/GUI/GUI.cpp'

# everything supports these (sort of)
subdir('Render/Null')
# mac doesnt support Vulkan but moltenVK is a thing so we might as well try that
subdir('Render/Vulkan')
# mac deprecated OpenGL a while ago but it still works so what the hell
subdir('Render/OpenGL')

sources += 'Volts/GUI/GlobalFrame.cpp'

if host_machine.system() == 'darwin'
    # metal is only on mac
    subdir('Render/Metal')
    sources += 'Volts/GUI/MacFrame.mm'
elif host_machine.system() == 'windows'
    sources += 'Volts/GUI/WindowsFrame.cpp'
else
    sources += 'Volts/GUI/LinuxFrame.cpp'
endif

# dx12 is only on windows
if host_machine.system() == 'windows'
    subdir('Render/DX12')
endif