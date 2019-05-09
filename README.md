# Volts
Yet another PS3 emulator

## Why?
Thought a complex project might be a good use of my time

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
ninja 
volt.exe
```

## Mac

As of May 9, 2019 supported MacOS versions are
* OSX 10.14 Mojave

```sh
git submodule update --init
meson Build
cd Build
ninja
./volt
```

## Linux

As of May 9, 2019 supported linux distros are
* Ubuntu 18.04.2 LTS

Assuming an entirley fresh install of linux

```sh
# Basic packages needed
sudo apt install git python3 python3-pip ninja-build clang cmake pkg-config zlib1g-dev
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
