subdir('imgui')

incs += include_directories('.')

sources += 'Volts/GUI/GUI.cpp'

# everything supports these (sort of)
subdir('Render/Null')
# mac doesnt support Vulkan but moltenVK is a thing so we might as well try that
subdir('Render/Vulkan')
# mac deprecated OpenGL a while ago but it still works so what the hell
subdir('Render/OpenGL')

if host_machine.system() == 'darwin'
    # metal is only on mac
    subdir('Render/Metal')
    sources += 'Volts/GUI/Frame.mm'
else
    sources += 'Volts/GUI/Frame.cpp'
endif

# dx12 is only on windows
if host_machine.system() == 'windows'
    subdir('Render/DX12')
endif