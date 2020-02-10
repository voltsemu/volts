#TODO: extract to subproject
libimgui = library('imgui', [
        'volts/external/imgui/imgui.cpp',
        'volts/external/imgui/imgui_draw.cpp',
        'volts/external/imgui/imgui_widgets.cpp',
        'volts/external/imgui/imgui_demo.cpp'
    ],
    include_directories : include_directories('imgui')
)

libfuzzy = library('fuzzy', [
        'volts/external/fts_fuzzy_match.cpp'
    ],
    include_directories : include_directories('.')
)

# subdir('aes')
# subdir('alert')

# gui_sources += [
#     'volts/external/fts_fuzzy_match.cpp'
# ]

# foreach src : [
#     'imgui_demo.cpp', 
#     'imgui_draw.cpp', 
#     'imgui.cpp', 
#     'imgui_widgets.cpp',
#     'examples/imgui_impl_glfw.cpp'
# ]
#     gui_sources += 'volts/external/imgui'/src
# endforeach

# if meson.get_compiler('cpp').has_header('vulkan/vulkan.h') and host_machine.system() != 'darwin'
#     sources += 'volts/external/imgui/examples/imgui_impl_vulkan.cpp'
#     args += '-DHAS_VULKAN=1'
# else
#     message('vulkan sdk not found')
# endif

# if host_machine.system() == 'windows'
#     gui_sources += 'volts/external/imgui/examples/imgui_impl_dx12.cpp'
#     links += 'd3d12.lib'
# elif host_machine.system() == 'darwin'
#     args += '-fobjc-arc'
#     gui_sources += 'volts/external/imgui/examples/imgui_impl_metal.mm'
# elif host_machine.system() == 'linux'
#     # todo: linux stuff
# endif

# incs += [
#     include_directories('imgui'),
#     include_directories('.')
# ]