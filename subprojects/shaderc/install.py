from zipfile import ZipFile
from urllib.request import urlopen

from sys import argv, exit
from io import BytesIO
from os.path import exists

dist = argv[1]
output = argv[2]

if exists(output):
    exit(0)

url = {
    'windows-debug': 'https://storage.googleapis.com/shaderc/artifacts/prod/graphics_shader_compiler/shaderc/windows/continuous_debug_2017/268/20200221-105547/install.zip',
    'darwin-debug': 'https://storage.googleapis.com/shaderc/badges/build_link_macos_clang_debug.html',
    'linux-debug': 'https://storage.googleapis.com/shaderc/badges/build_link_linux_clang_debug.html',

    'windows-release': 'https://storage.googleapis.com/shaderc/badges/build_link_windows_vs2017_release.html',
    'darwin-release': 'https://storage.googleapis.com/shaderc/badges/build_link_macos_clang_release.html',
    'linux-release': 'https://storage.googleapis.com/shaderc/badges/build_link_linux_clang_release.html'
}[dist]

resp = urlopen(url)
data = ZipFile(BytesIO(resp.read()))
data.extractall(output)
