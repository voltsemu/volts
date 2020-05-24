vk = dependency('vulkan', required : false)

if not vk.found()
    error('vulkan SDK not found, consider configuring with -Drender=disabled')
endif

sources += 'volts/rsx/vulkan/vk.cpp'
dependencies += vk

if get_option('buildtype').startswith('debug')
    cpp_args += '-DVK_VALIDATE=1'
endif
