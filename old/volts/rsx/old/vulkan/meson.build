vk = dependency('vulkan')

sources += 'volts/rsx/vulkan/render.cpp'
dependencies += vk

if get_option('buildtype').startswith('debug')
    cpp_args += '-DVK_VALIDATE=1'
endif
