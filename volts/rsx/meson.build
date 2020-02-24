include_directories += include_directories('.')

if host_machine.system() != 'darwin'
    subdir('ogl')

    subdir('vulkan')

    if host_machine.system() == 'windows'
        subdir('dx12')
    endif
else
    subdir('metal')
endif

dependencies += [ glfw, imgui, glm ]
sources += 'volts/rsx/render.cpp'
