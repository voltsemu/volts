# add all the sources needed
sources += [
    # data structures
    'Volts/PS3/Util/SFO.cpp',
    'Volts/PS3/Util/Decrypt/UNSELF.cpp',
    'Volts/PS3/Util/Decrypt/Keys.cpp',
    'Volts/PS3/Util/Decrypt/AES/aes.cpp',
    'Volts/PS3/Util/PUP.cpp',

    # CPU and memory stuff
    'Volts/PS3/VM/VM.cpp',
    'Volts/PS3/VM/Memory/Memory.cpp',
    'Volts/PS3/VM/Interpreter/PPUInterpreter.cpp',

    # GPU stuff
    'Volts/PS3/VM/Render/Frame.cpp',
    'Volts/PS3/VM/Render/OpenGL/Render.cpp',
    'Volts/PS3/VM/Render/Render.cpp',
]

if vulkan
    sources += [
        'Volts/PS3/VM/Render/Vulkan/Render.cpp',
        'Volts/PS3/VM/Render/Vulkan/VulkanSupport.cpp',
    ]
endif

if host_machine.system() == 'windows'
    # only windows supports directx
    sources += [
        'Volts/PS3/VM/Render/DirectX12/Render.cpp'
    ]
endif

# Mac specific, this has to be seperated here
# because compilers that arent clang flip when
# given .mm files
if host_machine.system() == 'darwin'
    sources += 'Volts/PS3/VM/Render/Metal/Render.mm'
endif

# add this directory to the include paths
incs += include_directories('../')