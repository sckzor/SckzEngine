# SckzEngine

A Vulkan games engine by Sckzor

## Progress on the engine

### 1st Priority

- [x] Basic Vulkan creation and initialization.
- [x] Proper makefile.
- [x] Basic destruction and memory leak prevention.
- [x] Change the "pipeline" class to "graphics pipeline" class and make the pipeline swapable.
- [x] Multiple Object Rendering.
- [x] Memory management.
- [x] The ability to move, scale and rotate objects at will instead of having them just spin as well as adding a camera class.
- [ ] Simple Lighting.
- [ ] Instanced/Batched Rendering.
- [ ] FBO filters.
- [ ] Finish the memory manager.
- [ ] Optimize, Optimize, Optimize.
- [ ] Simple GUIs.
- [ ] Advanced Lighting.
- [ ] Ray Tracing.
- [ ] Animation.
- [ ] Optimize, Optimize, Optimize.

### 2nd Priority

- [ ] Networking support.
- [ ] Video support for cut scenes and similar.
- [ ] Adding more classes... maybe. (perhaps this is a bad idea, don't add code you don't need?)
- [x] FORMATING!

### 3rd Priority

- [ ] DLSS support, if I can convince NVidia to give me the development materials.
- [ ] Support for other platorms.

## Known issues and bugs

- [ ] The sampler does not need to be recreated with every texture, one for all of the textures is fine.

## Build tools

- G++ is required to build the C++ code
- clang-format is needed to format all of the code that you might add

## Image of the Engine in action

![Screenshot](Screenshot.png)
