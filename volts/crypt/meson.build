libcrypt = library('crypt', [
        'sce.cpp',
        'self.cpp',
        'keys/keys.cpp',
    ],
    include_directories : [ include_directories('.') ],
    dependencies : [ aes, zlib, svl, spdlog, elf ]
)
