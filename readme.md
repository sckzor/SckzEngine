# SckzEngine

#### A Vulkan games engine by Sckzor

All Code in this library is (c) Sckzor 2021, All rights reserved!

## Goals for the engine

1. Learn to write extremely high performance code
2. Learn to implement advanced graphics features

## Progress on the engine

### 1st Priority

- [x] Basic Vulkan creation and initialization.
- [x] Proper makefile.
- [x] Basic destruction and memory leak prevention.
- [x] Change the "pipeline" class to "graphics pipeline" class and make the pipeline swapable.
- [x] Multiple Object Rendering.
- [x] Memory management.
- [x] The ability to move, scale and rotate objects at will instead of having them just spin as well as adding a camera class.
- [x] Add a render function that allows the scene to be rendered from multiple cameras without having the time advance.
- [ ] Simple Lighting.
- [x] Batched Rendering.
- [ ] Add sub-allocated buffers.
- [ ] FBO filters.
- [ ] Add an options for on-demand changes to the swap chain like locked refresh rate and stuff like that.
- [ ] Finish the _vulkan_ memory manager.
- [ ] Optimize, Optimize, Optimize.
- [ ] Simple GUIs.
- [ ] Advanced Lighting.
- [ ] Ray Tracing.
- [ ] Animation.
- [ ] Optimize, Optimize, Optimize.
- [ ] Add custom _system_ memory managment
- [ ] Add custom dynamic _system_ memory structures

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
- Valgrind for leak checking the code

## Latest Valgrind results

```
==31995== LEAK SUMMARY:
==31995==    definitely lost: 139,696 bytes in 19 blocks
==31995==    indirectly lost: 22,255 bytes in 312 blocks
==31995==      possibly lost: 0 bytes in 0 blocks
==31995==    still reachable: 184,876 bytes in 2,306 blocks
==31995==         suppressed: 0 bytes in 0 blocks
==31995==
==31995== ERROR SUMMARY: 99 errors from 80 contexts (suppressed: 0 from 0)
```

## Image of the Engine in action

![Screenshot](Screenshot.png)
