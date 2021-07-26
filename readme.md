# SckzEngine

#### A Vulkan games engine by Sckzor

All code in this repository is copyright Charles Sands 2021. All rights reserved! Ask me before copying.

This repository should only go public when I want to share it with someone, otherwise it should stay private most of the time.

Time behind schedule: 2 weeks

[Link to current branch's todo list](current-branch-todo.md)

## Goals for the engine

1. Learn to write extremely high performance code
2. Learn to implement advanced graphics features
3. Make a game that is fun to play

## Timeline for the game

- June '21 - Have the basic engine working including more advanced lighting and animation

  - ~~April 30, '21 - FBOs / Swapchain~~
  - ~~May 31, '21 - Memory Manager / GUIs / Particles~~
  - June 31, '21 - Animation / ~~More Lighting~~ / Shadows

- September '21 - Have a prototype for the game functioning, continue adding to the engine

- January '22 - Have the game finished and on Steam for others to enjoy

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
- [x] Simple Lighting.
- [x] Batched Rendering.
- [x] Add sub-allocated buffers.
- [x] FBO filters.
- [x] Add an options for on-demand changes to the swap chain like locked refresh rate and stuff like that.
- [x] Particles.
- [x] Finish the _vulkan_ memory manager.
- [x] Optimize, Optimize, Optimize.
- [x] Simple GUIs.
- [x] Advanced Lighting.
- [x] Bloom / Merge shader.
- [ ] Shadows!
- [x] Reflections.
- [x] Skybox.
- [x] FBO Class / Extensible pipelines for unique UBOs
- [ ] Animation.
- [ ] Error handling (out the wazoo!)
- [ ] Add custom _system_ memory managment
- [ ] Add custom dynamic _system_ memory structures

### 2nd Priority

- [ ] Networking support.
- [ ] Ray Tracing.
- [ ] Video support for cut scenes and similar.
- [ ] Adding more classes... maybe. (perhaps this is a bad idea, don't add code you don't need?)
- [x] FORMATING!

### 3rd Priority

- [ ] DLSS support, if I can convince NVidia to give me the development materials.
- [ ] Support for other platorms.

## Known issues and bugs

- [ ] The sampler does not need to be recreated with every texture, one for all of the textures is fine.
- [x] Some strange bug where if a time.deltatime call is made in the first iteration of the main while loop then the movement/ rotation of an object will just lockup and not move at all?
- [ ] Program breaks when the resolution becomes too large because the frame buffer gets bigger than the size of the memory blocks, I need to dynamically choose the size of the buffer based on the screen resolution. (maybe not an issue now?)
- [ ] The command pools bleed (but don't leak) memory because they are not garbage collected ever until the end of execution.
- [ ] Model should not destroy command buffers probably.
- [x] Due to an unknow issue in the filter and combine files the window resize is broken
- [ ] The bloom affect sometimes won't resize with the window... but it does most of the time.

## Build tools

- G++ is required to build the C++ code
- clang-format is needed to format all of the code that you might add
- Valgrind for leak checking the code
- VkTracer is needed for profiling Vulkan command and discovering performance sinks
- RenderDoc for profiling images and stuff

## Latest Valgrind results

```
==15983== LEAK SUMMARY:
==15983==    definitely lost: 58,608 bytes in 21 blocks
==15983==    indirectly lost: 33,199 bytes in 366 blocks
==15983==      possibly lost: 72,704 bytes in 1 blocks
==15983==    still reachable: 184,876 bytes in 2,306 blocks
==15983==         suppressed: 0 bytes in 0 blocks
```

## Cube map notes

```
      +-----+
      | -x  |  The arrows represent which way the bottom of the texture points
      |  v  |
+-----+-----+-----+-----+
| +z  | -y  |  -z |  +y |
|  >  |  o  |  <  |   o |
*-----+-----+-----+-----+
      |  ^  |
      | +x  |
      +-----+
```

I believe that all of the "memory leak" is caused by strage behavior of the Vulkan library... The errors though,
those were caused by me and I need to fix them at some point.

## Image of the Engine in action

![Screenshot](Screenshot.gif)
