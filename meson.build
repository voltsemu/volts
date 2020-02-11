project('volts', ['cpp', 'c'],
    default_options : [
        'cpp_std=c++17',
        'cpp_rtti=false',
        'cpp_eh=none',
        'default_library=static'
    ],
    version : '0.2.1',
    license : 'Apache'
)

if host_machine.system() == 'darwin'
    add_languages('objcpp')
endif

# TODO: using cpp_eh=none makes meson do funky stuff on windows, feels like a bug, should report
if meson.get_compiler('cpp').get_id() == 'msvc'
    add_global_arguments('/D_HAS_EXCEPTIONS=0', language : 'cpp')
endif

# collect all the dependencies that lots of things share

cmake = import('cmake')

zlib = dependency('zlib', fallback : [ 'zlib', 'zlib_dep' ])

glfw = dependency('glfw3', required : false)

if not glfw.found()
    glfw = cmake.subproject('glfw').dependency('glfw')
endif

spdlog_opts = [
    'enable_tests=false',
    'enable_examples=false',
    'no_exceptions=true',
    'no_thread_id=true',
    'no_tls=true'
]

if host_machine.system() == 'windows'
    spdlog_opts += 'wchar_support=true'
endif

spdlog = subproject('spdlog', default_options : spdlog_opts).get_variable('spdlog_dep')


xxhash = subproject('xxhash').get_variable('xxhash_dep')

rapidjson = subproject('rapidjson').get_variable('rapidjson_dep')

fuzzy = subproject('fts').get_variable('fuzzy_dep')

aes = subproject('aes').get_variable('aes_dep')

svl = subproject('svl').get_variable('svl_dep')

elf = subproject('elf').get_variable('elf_dep')

vfs = subproject('vfs').get_variable('vfs_dep')

alert = subproject('alert').get_variable('alert_dep')

imgui = subproject('imgui').get_variable('imgui_dep')

cxxopts = subproject('cxxopts').get_variable('cxxopts_dep')

# we structure the emulator by splitting it into core parts
# we have 
#   - a decryption library
#   - a loader library 
#   - a cpu emulation library <- loader
#   - a gpu emulation library <- cpu
#   - a frontend gui executable <- gpu
#   - a cli toolkit executable <- decryption

extra_deps = []

if host_machine.system() == 'darwin'
    extra_deps += dependency('appleframeworks', modules : [ 'Cocoa', 'Foundation' ])
endif

subdir('volts')


cli = executable('cli', [ 'volts/cli.cpp' ],
    link_with : [ libload, libcrypt ],
    dependencies : [ spdlog, svl, elf, vfs, cxxopts, rapidjson ]
)

gui = executable('gui', [ 'volts/gui.cpp' ],
    link_with : [ libgui ],
    dependencies : [ svl, vfs ] + extra_deps,
    gui_app : true
)

doxygen = find_program('doxygen', required : false)

if doxygen.found()
    run_target('docs', command : [doxygen, meson.source_root()/'Doxyfile'])
endif