# Volts
Yet another PS3 emulator

## Why?
Thought a complex project might be a good use of my time

# Coding style

We have an official code formatting style used throughout the codebase. As well as this we also have a coding style.

## Code formatting

4 space indents, not tabs, not 2 or 3 or 8 space indents.

all variables, classes, functions and templates must be in `PascalCase`. all template parameters must be prefixed with `T`.

Nested classes are outright banned, do not use them. ever.

All macros must be in `SCREAMING_SNAKE_CASE` they must also not pollute the function with variable names. be sure to surround each macro with `{}` if needed.

All enums must be `enum class` enums instead of `typedef enum` enums.

## Code style

code that produces warnings on any platform must be fixed to make the build completley quiet.

Performance is top priority, if something is not running at a satisfactory level optimize it.

External modules should not be added with gay abandon, if it is reasonable to implement the library within Volts do that instead of trying to get package managers to play nice.

Everything must be within the `Volts` namespace to avoid global namespace pollution.

Document any code that is even slightly ambiguous, if it isnt understandable in code review it must be fixed before it is pushed to master.

If it is a choice between OOP and procedural code, make procedural code.

exceptions are banned, no exceptions at all. ever.

# How to play a game
soon

# Building

Even if a specific linux distro, windows version or macosx version is not explicity supported does not mean it will not work. If you manage to boot the emulator as well as a game on your system please provide a pull request to update the system compatibility list.

## Windows

As of May 9, 2019 supported windows versions are
* Windows 10 Build 17763

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

Assuming an entirley fresh install of linux

```sh
# Basic packages needed
sudo apt install git python3 python3-pip ninja-build clang llvm
pip3 install --user meson

# Grab other stuff from github
git submodule update --init

# pip wont put meson on the path by default so we have to specifiy the full path
# meson will also use g++ by default, but g++ doesnt support c++17 as much as clang
# so we use clang instead
env CXX=clang++ ~/.local/bin/meson Build
cd Build
ninja
./volt 
```
