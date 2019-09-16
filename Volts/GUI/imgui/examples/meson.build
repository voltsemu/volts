# windows supports dx12 and win32, we add vulkan further down
if host_machine.system() == 'windows'
    sources += [
        'Volts/GUI/imgui/examples/imgui_impl_dx12.cpp',
        'Volts/GUI/imgui/examples/imgui_impl_win32.cpp'
    ]
endif

# only darwin supports metal
if host_machine.system() == 'darwin'
    sources += [
        'Volts/GUI/imgui/examples/imgui_impl_metal.mm',
        'Volts/GUI/imgui/examples/imgui_impl_osx.mm'
    ]
else
    # only darwin doesnt support vulkan
    sources += 'Volts/GUI/imgui/examples/imgui_impl_vulkan.cpp'
endif
