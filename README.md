# Pistachio

<p align="center">
    <image src="resources/Pistachio.png" alt="Pistachio">
</p>

A [Hazel](https://github.com/TheCherno/Hazel) Engine clone following the 
[Game Engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT) by 
[The Cherno](https://www.youtube.com/c/TheChernoProject). A simple 2D game engine.

Hat is the Game Editor GUI partner application for Pistachio.

## Setup

Pistachio is tested to work on **64-bit Windows** and **64-bit Linux Ubuntu 20.04**.

### Windows

1. Clone this repository
```ssh
git clone --recursive https://github.com/SigmathBits/Pistachio.git
```
2. Install the [VulkanSDK v1.3.204.1](https://vulkan.lunarg.com/sdk/home) or later for Windows.
3. Ensure you check "(Optional) Debuggable Shader API Libraries - 64-bit" at the **Select Components** step of installation if you require debug builds.
4. Run `scripts/Win-GenProject.bat` to generate the Visual Studio 2022 solution files.
5. Ensure you have the C++ [Visual Studio Build Tools 2022](https://visualstudio.microsoft.com/downloads/) installed
   (at the bottom under **Tools for Visual Studio**).
6. Open `Pistachio.sln` in [Visual Studio 2022](https://visualstudio.microsoft.com/vs/).

### Linux

1. Clone this repository
```ssh
git clone --recursive https://github.com/SigmathBits/Pistachio.git
```
2. Install the required dependencies
```ssh
sudo apt-get install make xorg-dev g++
```
3. Download the [VulkanSDK v1.3.204.1](https://vulkan.lunarg.com/sdk/home) tarball for Linux and extract the contents into a folder under `Pistachio/vendor/vulkan/`.
4. Change to the `Pistachio` top repository directory
```bash
cd Pistachio
```
5. Source the VulkanSDK setup script
```bash
source Pistachio/vendor/vulkan/1.3.204.1/setup-env.sh
```
6. Run premake to generate the Makefiles
```bash
./vendor/premake/bin/premake5 gmake2
```

## Build

Pistachio should build and run out of the box.

The first time Hat is run it can take 5–30 seconds to start the application while the shaders are first compiled and cached.

### Windows

Compile and run the program in Visual Studio (F5), Pistachio should compile, 
and the Hat editor should run and open automatically. 

### Linux

Run `make Hat` with the desired configuration (`make help` will list all the targets and configurations).
```bash
make Hat config=release
```
To run Hat, change to the `Hat` directory and run the executable generated under `bin`.
```bash
cd Hat
./../bin/Release-linux-x86_64/Hat/Hat
```

### Credits

[Pistachio](https://www.deviantart.com/jhayarr23/art/MLP-Vector-Pistachio-770159749) vector by 
[Jhayarr23](https://www.deviantart.com/jhayarr23).
