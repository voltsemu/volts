subdir('imgui')

incs += include_directories('.')

sources += 'Volts/GUI/GUI.cpp'

subdir('Render/Vulkan')

if host_machine.system() == 'darwin'
    sources += 'Volts/GUI/Frame.mm'
    subdir('Render/Metal')
else
    sources += 'Volts/GUI/Frame.cpp'
endif


if host_machine.system() == 'windows'
    subdir('Render/DX12')
endif