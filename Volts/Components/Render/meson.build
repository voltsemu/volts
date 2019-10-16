subdir('Vulkan')

if host_machine.system() != 'darwin'
    subdir('OpenGL')
endif

if host_machine.system() == 'windows'
    subdir('DX12')
elif host_machine.system() == 'darwin'
    subdir('Metal')
endif