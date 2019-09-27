# Volts
Yet another emulator

## Why?
So there can be another competing standard.

## Media
![Discord Shield](https://discordapp.com/api/guilds/578380242888949760/widget.png?style=shield)

# User Q&A

## How do I Run a game?

As of Sept 17, 2019. You don't. This emulator is still early stages.
If you want to play around with what does exist you're more than welcome.

# Roadmap

Planned features

## Shortterm features

* Decrypt SELF files and generate ELFs
* Working crossplatform ImGui
* Working CELL interpreter
* Minimal RSX implementation

## Longterm

* CELL JIT compiler
* Disk decryptor
* Controller input
* Performance improvments

## Sideprojects

* Themes
* Debugger

# Developer Q&A

## Code Style

Its important for a healthy code base to have a consistent style through all internal code.
This extends to design patterns as well as just the formatting of the code itself.

### Formatting

1. 4 space indents only, not tabs.
2. All `class`es, `struct`s, varaibles, functions, and arguments are in `PascalCase`
3. All template type arguments are prefixed with `T`
4. All macros are in `SCREAMING_SNAKE_CASE`
5. All enums are `enum class`es
6. `*` `&` are on the left side. IE `type* name`

### Patterns

1. All code is in the `Volts` namespace
2. No warnings when building
3. Performance above all else
4. `#pragma once` instead of include guards
5. All C++ have the `.cpp` file extension, C files have `.c`, ObjC++ files have `.mm`, all headers have `.h`
6. All code is documented clearly
7. External dependencies are minimal

### Bans

1. Exceptions are banned
2. `dynamic_cast` and RTTI are banned
3. `thread_local` is banned
4. Nested classes are banned
5. Relative imports are banned

### Building

Even if a specific Linux distro, Windows version, or MacOSX version is not listed here it does not mean it will not work.
If you successfully build & boot the emulator on your system please provide a pull request with detailed steps to update the README.

#### Prerequisites

* Python >= 3.5.x
* Meson >= 0.50.x
* Vulkan SDK >= 1.0.3 (set the enviroment variable "VULKAN_SDK" to the path to the installed sdk)

#### Windows

* Install visual studio C++ compiler (C++17 support or later)

```sh
meson Build
cd Build
ninja
volt.exe
```

#### MacOSX

As of Sept 17, 2019 supported MacOSX versions are
* OSX 10.14.6 Mojave

```sh
git submodule update --init
meson Build
ninja -C Build
./Build/volt
```

#### Linux

As of May 9, 2019 supported linux distros are
* Ubuntu 18.04.2 LTS
* Debian 9.9.0

Assuming an entirley fresh install of Ubuntu

```sh
sudo apt install git python3 python3-pip ninja-build
pip3 install --user meson

~/.local/bin/meson Build
ninja -C Build
./Build/volt
```

Assuming a fresh install of Debian

```sh
su -

apt install git python3-pip llvm ninja-build libx11-dev

pip3 install meson

cd Volts
meson Build
ninja -C Build
./Build/volt
```
