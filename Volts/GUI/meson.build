subdir('imgui')

incs += include_directories('.')

sources += 'Volts/GUI/GUI.cpp'

subdir('Render/Vulkan')

if host_machine.system() == 'darwin'
    sources += 'Volts/GUI/Frame.mm'
    deps += dependency('appleframeworks', modules : ['Foundation', 'Metal', 'MetalKit', 'QuartzCore', 'Cocoa'])
else
    sources += 'Volts/GUI/Frame.cpp'
endif


if host_machine.system() == 'windows'
    subdir('Render/DX12')
endif