rsx_src = []

rsx_deps = []

rsx_inc = [ include_directories('.') ]

if host_machine.system() != 'darwin'
    subdir('ogl')
endif

librsx = library('rsx', [ 'render.cpp' ] + rsx_src,
    dependencies : [ glfw, vfs, imgui, spdlog ] + rsx_deps,
    include_directories : rsx_inc
)