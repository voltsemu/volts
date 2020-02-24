vk = dependency('vulkan')

sources += 'volts/rsx/vulkan/render.cpp'
dependencies += vk

if get_option('buildtype').startswith('debug')
    cpp_args += '-DVK_VALIDATE=1'
endif

# find shaderc

python = import('python3').find_python()

if host_machine.system() == 'windows'
    vulkan_sdk = run_command(python, '-c', 'import os; print(os.environ["VULKAN_SDK"])').stdout().strip()

    #todo: the shipped shaderc with vulkan is release only so doesnt work well with meson
    # this is probably going to end up with us compiling glslang and shader ourselves
    #dependencies += meson.get_compiler('cpp').find_library('shaderc_combined', dirs : vulkan_sdk/'Lib')
endif