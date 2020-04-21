include_directories += include_directories('.')

subdir('vulkan')

if host_machine.system() == 'darwin'
    subdir('metal')
else
    subdir('opengl')

    if host_machine.system() == 'windows'
        subdir('directx12')
    endif
endif

sources += 'volts/rsx/render.cpp'
