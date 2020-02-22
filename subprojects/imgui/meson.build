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

deps = []

if host_machine.system() != 'darwin'
    sources += 'imgui/examples/imgui_impl_opengl3.cpp'
    
    glfw = dependency('glfw3', required : false)

    if not glfw.found()
        glfw = subproject('glfw').get_variable('glfw_dep')
    endif

    glad = subproject('glad').get_variable('glad_dep')

    deps += [ glfw, glad ]

endif

libimgui = library('imgui', sources,
    include_directories : inc,
    dependencies : deps,
    #TODO: should this be configurable?
    cpp_args : [ '-DIMGUI_IMPL_OPENGL_LOADER_GLAD' ],
    objcpp_args : [ '-fobjc-arc' ]
)

imgui_dep = declare_dependency(
    link_with : libimgui,
    include_directories : [ inc, include_directories('.') ]
)