vk = dependency('vulkan', required : false)

if vk.found()
    rsx_src += 'vulkan/render.cpp'
    rsx_args += '-DHAS_VULKAN=1'
    rsx_deps += vk

    if get_option('buildtype').startswith('debug')
        rsx_args += '-DVK_VALIDATE=1'
    endif
else
    message('vulkan not found, disabling')
endif