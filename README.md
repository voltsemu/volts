# Volts
Yet another emulator

## Why?
So there can be another competing standard.

## Media
![Discord Shield](https://discordapp.com/api/guilds/578380242888949760/widget.png?style=shield)
[![Build Status](https://travis-ci.com/Apache-HB/Volts.svg?token=dPzVKgEubgtf5tzmx1NP&branch=master)](https://travis-ci.com/Apache-HB/Volts)

# Coding style

It's important to have a consistent style througout the codebase to improve readability.

## Code formatting
1. 4 space indents, not tabs, not 2 or 3 or 8 space indents.
2. all variables, classes, functions and templates must be in `PascalCase`.
3. all template parameters must be prefixed with `T`.
4. All macros must be in `SCREAMING_SNAKE_CASE` they must also not pollute the function with variable names. be sure to surround each macro with `{}` if needed.
5. All enums must be `enum class` enums instead of `typedef enum` enums.
6. prefix all macros with `V`.

## Code style
1. Code that produces warnings on any platform must be fixed to make the build completley quiet.
2. Performance is top priority, if something is not running at a satisfactory level optimize it.
3. External modules should not be added with gay abandon, if it is reasonable to implement the library within Volts do that instead of trying to get package managers to play nice.
4. Everything must be within the `Volts` namespace to avoid global namespace pollution.
5. Document any code that is even slightly ambiguous, if it isnt understandable in code review it must be fixed before it is pushed to master.
6. If it is a choice between OOP and procedural code, make procedural code.

### Banned things
1. exceptions are banned, no exceptions at all. ever.
2. do not use RTTI or `dynamic_cast` ever, virtual functions should also be kept to a bare minimum.
3. `thread_local` is banned.
4. Nested classes are banned.

# How to play a game
soon

# Building

Even if a specific linux distro, windows version or macosx version is not explicity supported does not mean it will not work.
If you manage to boot the emulator as well as a game on your system please provide a pull request to update the system compatibility list.

## Windows

As of June 2, 2019 supported windows versions are
* Windows 10 Build 16299
* Windows 10 Build 17763

Assuming an entirley fresh install of Windows 10

1. Download [Git Bash](https://git-scm/download/win) or [GitHub Desktop](https://desktop.github.com/)
2. Clone Volts using either git bash or github desktop
3. Download [Visual Studio](https://visualstudio.microsoft.com/downloads/) and install `Desktop development with C++`
4. Download [python >= 3.6](https://www.python.org/downloads/). Be sure to install pip as part of the setup
5. TODO: once python on windows stops being so fucking painful

This has to be done in an `x64 Native Tools Command Promp for VS 20xx`
It will not work in any other command prompt.
This is a fault with meson rather than the volts codebase.
```sh
git submodule update --init
meson Build
cd Build
ninja msbuild
volt.exe
```

## Mac

As of May 9, 2019 supported MacOS versions are
* OSX 10.14 Mojave

```sh
git submodule update --init
meson Build
cd Build
ninja xcode
./volt
```

## Linux

As of May 9, 2019 supported linux distros are
* Ubuntu 18.04.2 LTS
* Debian 9.9.0

Assuming an entirley fresh install of Ubuntu

```sh
# Basic packages needed
sudo apt install git python3 python3-pip ninja-build
pip3 install --user meson

# pip wont put meson on the path by default so we have to specifiy the full path
# meson will also use g++ by default, but g++ doesnt support c++17 as much as clang
# so we use clang instead
~/.local/bin/meson Build
ninja -C Build
./Build/volt
```

Assuming a fresh install of debian

```sh
su -

apt install git python3-pip llvm ninja-build

meson Build

ninja -C Build

./Build/volt
```
