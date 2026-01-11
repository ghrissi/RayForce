# RayForce

## Description
Rayforce is a lightweight 3d physics game engine, with the purpose of having the maximum performance.

---
## Requirements & Dependencies

This project relies on the following core libraries:
* **Raylib:** Used for rendering and window management.
* **NVIDIA PhysX:** Utilized for high-performance physics simulations.

---
## Installation

To get a local copy up and running, follow these steps:

### 1. Clone the Repository
Clone the project using Git:
`git clone https://github.com/Alvareos11/RayForce.git`

### 2. Install extern Dependencies

**Visual Estudio 2022:**
First open **RayForce.sln** with Visual Studio, then run the comand
`donet restore`.

**Others IDEs:**
Follow the installation documentation of the extern Dependencies for your OS:
* **Raylib:** Refer to the [Raylib Wiki: Working on Platforms](https://github.com/raysan5/raylib/wiki/Working-on-Platforms) for detailed installation guides on Windows (MinGW/MSVC), Linux, and macOS.
* **NVIDIA PhysX:** Follow the build and setup instructions in the [NVIDIA PhysX Official Repository](https://github.com/NVIDIA-Omniverse/PhysX#quick-start). You will need to compile the SDK for your specific platform to obtain the necessary libraries.

### 3. Compilation

**Visual Studio 2022** 
Open **RayForce.sln** and the press F5.

**Cmake (Cross-Platform):**
Run the following comands from the root directory:
`cd RayForce`
`mkdir build`
`cd build`
`cmake ..`
`make`.

---
### Troubleshooting
**On Linux / Raspberry Pi:**
If the engine fails to start, force the OpenGL version by running:
`MESA_GL_VERSION_OVERRIDE=3.3 ./RayForce`

## Acknowledgments and Legal Notices

**RayForce** is an open-source project that integrates third-party technologies. By using this software, you acknowledge and agree to the license terms of the following components:

### NVIDIA® PhysX® SDK
This project uses the **NVIDIA® PhysX® SDK**.
- Copyright © 2026 NVIDIA Corporation. All rights reserved.
- Licensed under the **BSD-3-Clause License**.
- NVIDIA® and PhysX® are registered trademarks of NVIDIA Corporation.

### raylib
This project uses **raylib** for window management and rendering.
- Copyright © 2013-2026 Ramon Santamaria (@raysan5).
- Licensed under the **zlib/libpng License**.

## License
**RayForce** is licensed under the **GNU General Public License v3.0 (GPL-3.0)**. 
Copyright © 2026 [Your Name].

### glTF Sample Models
This project uses 3D models from the [glTF Sample Models](https://github.com/KhronosGroup/glTF-Sample-Models) repository by Khronos Group.
- These models are used for testing and demonstration purposes.
- Licensed under [CC-BY 4.0](https://creativecommons.org/licenses/by/4.0/).
