# Volts
Yet another emulator

## Why?
So there can be another competing standard.

## Media
![Discord Shield](https://discordapp.com/api/guilds/578380242888949760/widget.png?style=shield)

# User Q&A

Windows GUI requires Windows 10 version 1909 at least
Mac GUI requires osx 10.15 at least
Linux GUI is not currently planned for development but the cli should work file

## How do I Run a game?

As of Sept 17, 2019. You don't. This emulator is still early stages.
If you want to play around with what does exist you're more than welcome.

# Roadmap

Planned features

## Shortterm features

* ~~Decrypt SELF files and generate ELFs~~
* Working CELL interpreter
* Minimal RSX implementation
* Get the test cube rpcs3 has to boot and not crash (it can crash on exit and thats still a win)

## Longterm

* CELL JIT compiler
* Disk decryptor
* Controller input
* Performance improvments
* Code tidying
* AES-NI accellerated game decryption (probably use openssl)

## Sideprojects

* Debugger
* Native GUI frontends, xaml islands for windows, swiftui on macos, gtk or the like on linux
* Github CI
* Replace zlib with something modern
* Get glslang to properly unity build
* Start building with `warning_level=3`
* Get full project unity builds working on all platforms (zlib has to go if we try this)

# Developer Q&A

## Code Style

Its important for a healthy code base to have a consistent style through all internal code.
This extends to design patterns as well as just the formatting of the code itself.

### Formatting

1. 4 space indents only, not tabs.
2. All `class`es, `struct`s, varaibles, functions, and arguments are in `snake_case`
3. All template type arguments are prefixed with `T`
4. All macros are in `SCREAMING_SNAKE_CASE`
5. All enums are `enum class`es
6. `*` `&` are on the left side. IE `type* name`

### Patterns

1. All code is in the `volts` namespace
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
5. Relative includes are banned

### Building

See [BUILD.md](BUILD.md) for instructions for your operating system and a list of supported systems

### Comparison

#### Volts

##### Pros
- No warnings while building
- Uses meson
- Fully cross platform
- Has command line utilities built in

##### Cons
- Cant run ps3 games
- Is nowhere near finished

#### rpcs3

##### Pros
- Can run ps3 games
- Patch support

##### Cons
- Uses cmake and visual studio
- No command line interface
- An almost uncountable amount of build warnings
- Doesnt work on mac
- Uses Qt
- Inconsistent code style
