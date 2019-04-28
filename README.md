# Volts
A PS3 emulator kinda like RPCS3

## Why?
RPCS3s code is a bit too noodly for my taste, thought i'd give it a shot myself.

# Building

## Windows

### Required tools
1. `choco` to install all the other packages
2. `powershell` the command prompt is not supported
3. `nuget` to install dependancies
4. `git` to install submodules
5. `python3` to run meson
6. `meson` to build the emulator itself
7. `visual studio` for building the native parts for windows using `msbuild`

### Building

#### Setup

1. Install Visual Studio `https://visualstudio.microsoft.com/vs/`
2. Install choco 
```sh
Set-ExecutionPolicy Bypass -Scope Process -Force; iex ((New-ObjectSystem.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
```

3. Install deps
```sh
choco install meson git 
```

4. Download subprojects

```sh
git submodule update --init
```


2. actually building
```sh
cd Path/To/Volts
meson Build
cd Build
ninja
./Volts.exe # or find and double click the .exe
```

## Mac
soon

## Linux
soon