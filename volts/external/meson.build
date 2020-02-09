subdir('aes')

gui_sources += [
    'volts/external/fts_fuzzy_match.cpp'
]

foreach src : [
    'imgui_demo.cpp', 
    'imgui_demo.cpp', 
    'imgui.cpp', 
    'imgui_widgets.cpp',
    'examples/imgui_impl_glfw.cpp',
    'examples/imgui_impl_vulkan.cpp'
]
    gui_sources += 'volts/external/imgui'/src
endforeach

if host_machine.system() == 'windows'
    gui_sources += 'volts/external/imgui/examples/imgui_impl_dx12.cpp'
elif host_machine.system() == 'darwin'
    gui_sources += 'volts/external/imgui/examples/imgui_impl_metal.mm'
elif host_machine.system() == 'linux'
    # todo: linux stuff
endif

incs += include_directories('imgui')