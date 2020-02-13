rsx_src = []

rsx_args = []

rsx_deps = []

rsx_inc = [ include_directories('.') ]

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

librsx = library('rsx', [ 'render.cpp' ] + rsx_src,
    dependencies : [ glfw, vfs, imgui, spdlog, browser ] + rsx_deps,
    include_directories : rsx_inc,
    objcpp_args : [ '-std=c++17' ],
    cpp_args : rsx_args
)