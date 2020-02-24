project('glfw', 'c')

sources = []
dependencies = []
args = []

if host_machine.system() == 'darwin'
    add_languages('objc')

    dependencies += dependency('appleframework', modules : [ 'Cocoa', 'IOKit', 'CoreFoundation' ])

    sources += [
        'glfw/src/cocoa_init.m',
        'glfw/src/cocoa_joystick.m',
        'glfw/src/cocoa_monitor.m',
        'glfw/src/cocoa_window.m',
        'glfw/src/cocoa_time.c',
        'glfw/src/posix_thread.c',
        'glfw/src/nsgl_context.m',
        'glfw/src/egl_context.c',
        'glfw/src/osmesa_context.c'
    ]

    args += '-D_GLFW_COCOA'
elif host_machine.system() == 'windows'
    sources += [
        'glfw/src/win32_init.c',
        'glfw/src/win32_joystick.c',
        'glfw/src/win32_monitor.c',
        'glfw/src/win32_time.c',
        'glfw/src/win32_thread.c',
        'glfw/src/win32_window.c',
        'glfw/src/wgl_context.c',
        'glfw/src/egl_context.c',
        'glfw/src/osmesa_context.c'
    ]

    args += [ '/D_GLFW_WIN32' ]
elif host_machine.system() == 'linux'
    sources += [
        'glfw/src/x11_init.c',
        'glfw/src/x11_monitor.c',
        'glfw/src/x11_window.c',
        'glfw/src/xkb_unicode.c',
        'glfw/src/posix_time.c',
        'glfw/src/posix_thread.c',
        'glfw/src/glx_context.c',
        'glfw/src/egl_context.c',
        'glfw/src/osmesa_context.c'
    ]

    args += '-D_GLFW_X11'
endif

sources += [
    'glfw/src/context.c',
    'glfw/src/init.c',
    'glfw/src/input.c',
    'glfw/src/monitor.c',
    'glfw/src/vulkan.c',
    'glfw/src/window.c'
]

libglfw = library('glfw3', sources,
    include_directories : include_directories('glfw/include'),
    dependencies : dependencies,
    c_args : args,
    cpp_args : args,
    objc_args : args,
    objcpp_args : args
)

glfw_dep = declare_dependency(
    link_with : libglfw,
    include_directories : include_directories('glfw/include')
)