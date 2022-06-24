# Pistachio

<p align="center">
    <image src="resources/Pistachio.png" alt="Pistachio">
</p>

A [Hazel](https://github.com/TheCherno/Hazel) Engine clone following the 
[Game Engine series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT) by 
[The Cherno](https://www.youtube.com/c/TheChernoProject). A simple 2D game engine.

Hat is the Game Editor GUI partner application for Pistachio.

## Setup

Currently Pistachio is only tested to work for **64-bit Windows**.

1. Clone this repository
```ssh
git clone --recursive https://github.com/SigmathBits/Pistachio.git
```
2. Install the [VulkanSDK v1.3.204.1](https://vulkan.lunarg.com/sdk/home) or later for Windows.
3. Ensure you check "(Optional) Debuggable Shader API Libraries - 64-bit" at the **Select Componenents** step of installation if you require debug builds.
4. Run `scripts/Win-GenProject.bat` to generate the Visual Studio 2022 solution files.
5. Ensure you have the C++ [Visual Studio Build Tools 2022](https://visualstudio.microsoft.com/downloads/) installed
   (at the bottom under **Tools for Visual Studio**).
6. Open `Pistachio.sln` in [Visual Studio 2022](https://visualstudio.microsoft.com/vs/).

## Build

Pistachio should build and run out of the box. Compile and run the program in Visual Studio (F5), Pistachio should compile, 
and the Hat editor should run and open automatically. 

The first run can take 5–30 seconds to start the application while the shaders are first compiled and cached.

### Credits

[Pistachio](https://www.deviantart.com/jhayarr23/art/MLP-Vector-Pistachio-770159749) vector by
[Jhayarr23](https://www.deviantart.com/jhayarr23).
