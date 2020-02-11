project('imgui', 'cpp',
    default_options : [
        'cpp_std=c++11'
    ]
)

inc = include_directories('imgui')

sources = [
    'imgui/imgui.cpp',
    'imgui/imgui_draw.cpp',
    'imgui/imgui_demo.cpp',
    'imgui/imgui_widgets.cpp',
    'imgui/examples/imgui_impl_glfw.cpp'
]

if host_machine.system() == 'windows'
    sources += 'imgui/examples/imgui_impl_dx12.cpp'
elif host_machine.system() == 'darwin'
    add_languages('objcpp')
    sources += 'imgui/examples/imgui_impl_metal.mm'
else
    #TODO: linux 
endif

glfw = dependency('glfw3', required : false)

if not glfw.found()
    cmake = import('cmake')
    glfw = cmake.subproject('glfw').dependency('glfw')
endif

libimgui = library('imgui', sources,
    include_directories : inc,
    dependencies : glfw,
    objcpp_args : [ '-fobjc-arc' ]
)

imgui_dep = declare_dependency(
    link_with : libimgui,
    include_directories : [ inc, include_directories('.') ]
)