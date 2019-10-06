
vk_sources = [
    'Volts/GUI/Render/Vulkan/Render.cpp'
]

if host_machine.system() == 'darwin'
    mvk_path = '/usr'/'local'/'Cellar'/'molten-vk'/'1.0.34'
    if run_command('[', '-d', mvk_path, ']').returncode() != 0
        message('molten-vk not detected, install with homebrew manually')
    else
        deps += dependency('appleframeworks', modules : [ 'IOSurface', 'IOKit' ])
        sources += vk_sources
        incs += include_directories(mvk_path/'libexec'/'include')
        links += mvk_path/'lib'/'libMoltenVK.a'
    endif
else
    pymodule = import('python3')
    python3 = pymodule.find_python()

    #check if we have the vulkan sdk
    res = run_command(python3, '-c', 'import os; print(os.environ["VULKAN_SDK"])')

    # returns non-zero because an exception is thrown if the sdk isnt found
    if res.returncode() != 0
        message('Vulkan SDK not found, not building with vulkan')
    else
        # if we do then we should use it
        incs += res.stdout().strip() / 'Include'
        links += res.stdout().strip() / 'Lib' / 'vulkan-1.lib'

        sources += vk_sources

        if host_machine.system() == 'windows'
            args += '-DVK_USE_PLATFORM_WIN32_KHR'
        endif
    endif
endif