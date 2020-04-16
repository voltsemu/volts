project('glslang', [ 'c', 'cpp' ],
    default_options : [
        'cpp_std=c++11',
        'cpp_eh=none',
        'cpp_rtti=false'
    ]
)

cpp_args = [
    '-DNV_EXTENSIONS',
    '-DAMD_EXTENSIONS'
]

# glsl sources
sources = [
    'glslang/glslang/MachineIndependent/glslang_tab.cpp',
    'glslang/glslang/MachineIndependent/attribute.cpp',
    'glslang/glslang/MachineIndependent/Constant.cpp',
    'glslang/glslang/MachineIndependent/iomapper.cpp',
    'glslang/glslang/MachineIndependent/InfoSink.cpp',
    'glslang/glslang/MachineIndependent/Initialize.cpp',
    'glslang/glslang/MachineIndependent/IntermTraverse.cpp',
    'glslang/glslang/MachineIndependent/Intermediate.cpp',
    'glslang/glslang/MachineIndependent/ParseContextBase.cpp',
    'glslang/glslang/MachineIndependent/ParseHelper.cpp',
    'glslang/glslang/MachineIndependent/PoolAlloc.cpp',
    'glslang/glslang/MachineIndependent/RemoveTree.cpp',
    'glslang/glslang/MachineIndependent/Scan.cpp',
    'glslang/glslang/MachineIndependent/ShaderLang.cpp',
    'glslang/glslang/MachineIndependent/SymbolTable.cpp',
    'glslang/glslang/MachineIndependent/Versions.cpp',
    'glslang/glslang/MachineIndependent/intermOut.cpp',
    'glslang/glslang/MachineIndependent/limits.cpp',
    'glslang/glslang/MachineIndependent/linkValidate.cpp',
    'glslang/glslang/MachineIndependent/parseConst.cpp',
    'glslang/glslang/MachineIndependent/reflection.cpp',
    'glslang/glslang/MachineIndependent/preprocessor/Pp.cpp',
    'glslang/glslang/MachineIndependent/preprocessor/PpAtom.cpp',
    'glslang/glslang/MachineIndependent/preprocessor/PpContext.cpp',
    'glslang/glslang/MachineIndependent/preprocessor/PpScanner.cpp',
    'glslang/glslang/MachineIndependent/preprocessor/PpTokens.cpp',
    'glslang/glslang/MachineIndependent/propagateNoContraction.cpp',
    'glslang/glslang/GenericCodeGen/CodeGen.cpp',
    'glslang/glslang/GenericCodeGen/Link.cpp',

    'glslang/OGLCompilersDLL/InitializeDll.cpp'
]

include_directories = [
    include_directories('glslang/glslang'),
    include_directories('glslang')
]

# spirv sources
sources += [
    'glslang/SPIRV/GlslangToSpv.cpp',
    'glslang/SPIRV/InReadableOrder.cpp',
    'glslang/SPIRV/Logger.cpp',
    'glslang/SPIRV/SpvBuilder.cpp',
    'glslang/SPIRV/SpvPostProcess.cpp',
    'glslang/SPIRV/doc.cpp',
    'glslang/SPIRV/disassemble.cpp'
]

if host_machine.system() == 'windows'
    cpp_args += [
        '-DGLSLANG_OSINCLUDE_WIN32',
        '/wd4146'
    ]
    sources += 'glslang/glslang/OSDependent/Windows/ossource.cpp'
else
    cpp_args += [
        '-DGLSLANG_OSINCLUDE_UNIX',
        '-Wno-unused-const-variable'
    ]
    sources += 'glslang/glslang/OSDependent/Unix/ossource.cpp'
endif

libglsl = library('glsl', sources,
    include_directories : include_directories,
    cpp_args : cpp_args
)

glsl_dep = declare_dependency(
    link_with : libglsl,
    include_directories : include_directories
)