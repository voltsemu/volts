subdir('imgui')

incs += include_directories('.')

if host_machine.system() == 'darwin'
    sources += 'Volts/GUI/Frame.mm'
else
    sources += 'Volts/GUI/Frame.cpp'
    subdir('Render/Vulkan')
endif
