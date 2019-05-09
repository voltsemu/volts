# Volts
The other PS3 emulator

## Why?
Because I felt like it.

# How to play a game
soon

# Building

## Windows
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
```sh
git submodule update --init
meson Build
cd Build
ninja
./volt
```

## Linux

Assuming an entirley fresh install of 64 bit Ubuntu 18.04.2 LTS.
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
