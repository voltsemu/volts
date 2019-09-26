subdir('imgui')

incs += include_directories('.')

sources += 'Volts/GUI/GUI.cpp'

# get the null renderer
subdir('Render/Null')

# get the vulkan renderer
subdir('Render/Vulkan')

subdir('Render/OpenGL')

if host_machine.system() == 'darwin'
    # metal is only on mac
    subdir('Render/Metal')
    sources += 'Volts/GUI/Frame.mm'
    deps += dependency('appleframeworks', modules : ['Foundation', 'Metal', 'MetalKit', 'QuartzCore', 'Cocoa'])
else
    sources += 'Volts/GUI/Frame.cpp'
endif

# dx12 is only on windows
if host_machine.system() == 'windows'
    subdir('Render/DX12')
endif