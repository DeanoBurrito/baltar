# Baltar
Baltar is a terminal emulator written in C++ with gpu accelerated rendering, inspired by alacritty. It's a hobby project and certainly not production ready, but feel free to try it out yourself!

## Features
- GPU accelerated rendering

## Screenshots
*coming soon*

## Resources Used
- Freetype, used for parsing and rendering fonts into greyscale bitmaps used for rendering.
- OpenGL ES 3.0 API. My initial target was ES 2.0 (like alacritty) but ES3 provides a more modern (and much more flexible) API over ES2. It'd likely be possible to port the renderer to use ES 2.0 if you *really* wanted to, but I do not.
- GLFW, used for window management and platform-specific parts of OpenGL like creating and managing contexts.
- GLAD, loads OpenGL driver functions at runtime and discovers available extensions.
- GLM (header only), a nice maths library that easily works with builtin GLSL datatypes.
- Nanoprintf, small and fast printf library.

## Building It Yourself
The following dependencies are required for building:
- A C++17 compiler, by default the makefile uses `g++`, but any standard compliant compiler should work fine.
- Development versions of the following packages: glm, glfw and freetype2 (might just be called freetype in your package manager).
- An OpenGL ES 3.0 capable graphics card. This should be nearly anything post 2012, although early hardware may be able to use mesa3D to emulate ES3 in software.
- Some system headers are used for managing PTYs, but any linux/BSD system should have these installed.
