# windows supports dx12 and win32, we add vulkan further down
if host_machine.system() == 'windows'
    sources += [
        'Volts/GUI/imgui/examples/imgui_impl_dx12.cpp',
        'Volts/GUI/imgui/examples/imgui_impl_win32.cpp'
    ]
    links += ['d3d12.lib', 'd3dcompiler.lib']
endif

# only darwin supports metal
if host_machine.system() == 'darwin'
    sources += [
        'Volts/GUI/imgui/examples/imgui_impl_metal.mm',
        'Volts/GUI/imgui/examples/imgui_impl_osx.mm'
    ]
endif