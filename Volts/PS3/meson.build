ps3_sources = [
    'Util/SFO.cpp',
    'Util/Decrypt/UNSELF.cpp',
    'Util/Decrypt/Keys.cpp',
    'Util/Decrypt/AES/aes.cpp'
]

libs += static_library('PS3', ps3_sources, 
    include_directories : [ 
        include_directories('../'),
        cthulhu.get_variable('inc') 
    ],
    link_with : [
        cthulhu.get_variable('metalib'),
        cthulhu.get_variable('corelib'),
        cthulhu.get_variable('fslib')
    ]
)