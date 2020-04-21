vk = dependency('vulkan')

sources += 'volts/rsx/vulkan/vk.cpp'
dependencies += vk

if get_option('buildtype').startswith('debug')
    cpp_args += '-DVK_VALIDATE=1'
endif
