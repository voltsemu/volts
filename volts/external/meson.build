subdir('aes')
subdir('alert')

gui_sources += [
    'volts/external/fts_fuzzy_match.cpp'
]

foreach src : [
    'imgui_demo.cpp', 
    'imgui_draw.cpp', 
    'imgui.cpp', 
    'imgui_widgets.cpp',
    'examples/imgui_impl_glfw.cpp'
]
    gui_sources += 'volts/external/imgui'/src
endforeach

if meson.get_compiler('cpp').has_header('vulkan/vulkan.h')
    gui_sources += 'volts/external/imgui/examples/imgui_impl_vulkan.cpp'
    args += '-DHAS_VULKAN=1'
else
    message('vulkan sdk not found')
endif

if host_machine.system() == 'windows'
    gui_sources += 'volts/external/imgui/examples/imgui_impl_dx12.cpp'
    links += 'd3d12.lib'
elif host_machine.system() == 'darwin'
    gui_sources += 'volts/external/imgui/examples/imgui_impl_metal.mm'
elif host_machine.system() == 'linux'
    # todo: linux stuff
endif

incs += include_directories('imgui')