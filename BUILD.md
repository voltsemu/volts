
### Building

Even if a specific Linux distro, Windows version, or MacOSX version is not listed here it does not mean it will not work.
If you successfully build & boot the emulator on your system please provide a pull request with detailed steps to update the README.

#### Prerequisites

* Python >= 3.5.x
* Meson >= 0.50.x
* Vulkan SDK >= 1.0.3 (on windows & linux set the enviroment variable "VULKAN_SDK" to the path to the installed sdk. on mac who really knows, just try and install the SDK. only god knows how to do this) [Download](https://vulkan.lunarg.com/sdk/home)

#### Windows

* Install visual studio C++ compiler (C++17 support or later)

```sh
git submodule update --init
meson build
cd build
ninja
volt.exe
```

#### MacOSX

This will only build on OSX 10.15 (Catalina) or later

you will also need to install xcode 11.1

```sh
git submodule update --init
meson build
ninja -C build
./build/volt
```

#### Linux

As of May 9, 2019 supported linux distros are
* Ubuntu 18.04.2 LTS
* Ubuntu 20.02
* Debian 9.9.0
* WSL Ubuntu 18.04
* WSL Ubuntu 20.04

#### Ubuntu 18.04

```sh
sudo apt update
sudo apt install git python3 python3-pip ninja-build zlib1g-dev pkg-config vulkan-sdk
pip3 install --user meson
git submodule update --init

~/.local/bin/meson build
ninja -C build
./build/cli
```

#### Debian 9

```sh
su -

apt install git python3-pip llvm ninja-build zlib1g-dev pkg-config
git submodule update --init

pip3 install meson

meson build
ninja -C build
./build/cli
```

#### WSL Ubuntu 20.04 LTS

```sh
sudo apt update
sudo apt upgrade
sudo apt install clang-10 meson zlib1g-dev pkg-config

CC=clang-10 CXX=clang++-10 meson build
ninja -C build
```