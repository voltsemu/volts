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

zlib = dependency('zlib', fallback : [ 'zlib', 'zlib_dep' ])

glfw = dependency('glfw3', required : false)

if not glfw.found()
    glfw = subproject('glfw').get_variable('glfw_dep')
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

glm = subproject('glm').get_variable('glm_dep')

glsl = subproject('glslang').get_variable('glsl_dep')

#toml = subproject('toml').get_variable('toml_dep')

links = []
sources = []
dependencies = [ aes, xxhash, rapidjson, svl, elf, vfs, imgui, cxxopts, spdlog, zlib, glsl ]
cpp_args = [ '-DFMT_EXCEPTIONS=0' ]
include_directories = []

if host_machine.system() == 'darwin'
    dependencies += dependency('appleframeworks', modules : [ 'Cocoa', 'Foundation' ])
endif

if host_machine.system() != 'windows'
    cpp_args += [ '-std=c++17' ]
endif

subdir('volts')

libvolts = library('volts', sources,
    cpp_args : cpp_args,
    objcpp_args : cpp_args,
    include_directories : include_directories,
    dependencies : dependencies,
    link_args : links
)

cli = executable('cli', 'volts/cli.cpp',
    cpp_args : cpp_args,
    objcpp_args : cpp_args,
    include_directories : include_directories,
    dependencies : dependencies,
    link_with : libvolts,
    link_args : links
)

doxygen = find_program('doxygen', required : false)

if doxygen.found()
    run_target('docs', command : [doxygen, meson.source_root()/'Doxyfile'])
endif