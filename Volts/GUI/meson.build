subdir('imgui')

incs += include_directories('.')

sources += 'Volts/GUI/GUI.cpp'

if host_machine.system() == 'darwin'
    sources += 'Volts/GUI/Frame.mm'
else
    sources += 'Volts/GUI/Frame.cpp'
    subdir('Render/Vulkan')
endif


if host_machine.system() == 'windows'
    subdir('Render/DX12')
endif