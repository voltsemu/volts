vk = dependency('vulkan', required : false)

if vk.found()
    cpp_args += '-DHAS_VULKAN=1'
    sources += 'volts/rsx/vulkan/render.cpp'
    dependencies += vk

    if get_option('buildtype').startswith('debug')
        cpp_args += '-DVK_VALIDATE=1'
    endif
else
    message('vulkan not found, disabling')
endif