# Building

## Windows
* Install [VisualStudio](https://visualstudio.microsoft.com/downloads/)
* Install [meson](https://mesonbuild.com/Getting-meson.html)
* Install the [vulkan sdk](https://www.lunarg.com/vulkan-sdk/)
* Install git

Open `x64 Native Tools Command Prompt`
```bat
git clone https://github.com/voltsemu/volts --depth 1
cd volts
git submodule update --init --depth 1
meson build
ninja -C build
```

## Linux
```sh
sudo apt install git meson build-essential

git clone https://github.com/voltsemu/volts --depth 1
cd volts
git submodule update --init --depth 1
meson build
ninja -C build
```

## Mac
* Install [brew](https://brew.sh/)
* Install Xcode 15
```sh
brew install git meson

git clone https://github.com/voltsemu/volts --depth 1
cd volts
git submodule update --init --depth 1
meson build
ninja -C build
```