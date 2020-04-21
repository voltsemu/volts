sources += 'volts/rsx/directx12/dx12.cpp'

links += [ 'd3d12.lib', 'dxgi.lib' ]

if get_option('buildtype').startswith('debug')
    cpp_args += '-DDX_VALIDATE=1'
    links += 'dxguid.lib'
endif