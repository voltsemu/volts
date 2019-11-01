incs += include_directories('.')

sources += [
    'Volts/External/imgui/imgui.cpp',
    'Volts/External/imgui/imgui_draw.cpp',
    'Volts/External/imgui/imgui_widgets.cpp',
    'Volts/External/imgui/imgui_demo.cpp'
]

subdir('examples')

if host_machine.system() == 'darwin'
    deps += dependency('appleframeworks', modules : [ 'Foundation', 'Cocoa', 'Metal', 'MetalKit' ])
endif

