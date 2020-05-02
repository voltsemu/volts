project('zlib-ng', 'c')

sources = [
    'zlib-ng/x86/crc_folding.c',
    'zlib-ng/x86/deflate_quick.c',
    'zlib-ng/x86/fill_window_sse.c',
    'zlib-ng/x86/insert_string_sse.c',
    'zlib-ng/x86/slide_avx.c',
    'zlib-ng/x86/slide_sse.c',
    'zlib-ng/x86/x86.c',
    'zlib-ng/adler32.c',
    'zlib-ng/compress.c.c',
    'zlib-ng/crc32.c',
    'zlib-ng/deflate_fast.c',
    'zlib-ng/deflate_medium.c',
    'zlib-ng/deflate_slow.c',
    'zlib-ng/deflate.c'
]

args = [
    '-DX86_CPUID',
    '-DX86_AVX2',
    '-DX86_SSE42_CRC_HASH',
    '-DX86_QUICK_STRATEGY',
    '-DX86_NOCHECK_SSE2',
    '-DX86_SSE42_CRC_INTRIN',
    '-DX86_PCLMULQDQ_CRC'
]

libzlib_ng = library('zlib-ng', sources,
    c_args : args,
    include_directories : include_directories('zlib-ng')
)

zlib_dep = declare_dependency(
    link_with : libzlib_ng,
    include_directories : include_directories('.')
)