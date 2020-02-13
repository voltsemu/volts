vk = dependency('vulkan', required : false)

if vk.found()
    rsx_src += 'vulkan/render.cpp'
    rsx_args += '-DHAS_VULKAN=1'
else

endif