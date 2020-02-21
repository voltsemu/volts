include_directories += include_directories('.')

if host_machine.system() != 'darwin'
    subdir('ogl')

    #TODO: vulkan is hot garbage and i hate it so this thing
    # is going to stay stubbed until someone else does it for me
    subdir('vulkan')

    if host_machine.system() == 'windows'
        subdir('dx12')
    endif
else
    subdir('metal')
endif

dependencies += [ glfw, imgui, glm ]
sources += 'volts/rsx/render.cpp'
