include_directories += include_directories('.')

subdir('vulkan')

if host_machine.system() == 'darwin'
    subdir('metal')
elif host_machine.system() == 'windows'
    subdir('directx12')
endif

sources += 'volts/rsx/render.cpp'
