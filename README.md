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
```sh
git submodule update --init
meson Build
cd Build
ninja
./volt 
```
