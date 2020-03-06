sources += 'volts/rsx/dx12/render.cpp'

links += [ 'd3d12.lib', 'dxgi.lib' ]

if get_option('buildtype').startswith('debug')
    cpp_args += '-DDX_VALIDATE=1'
endif