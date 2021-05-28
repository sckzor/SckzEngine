# SckzEngine

#### A Vulkan games engine by Sckzor

All code in this repository is copyright Charles Sands 2021. All rights reserved!

## To do

#### Things to do in the current branch before it is merged to master

- [ ] Add UBO support to the "FBO shader" (which should be renamed).
- [x] Make it so either the image is rendered to an off screen buffer, or (probably the better idea) make it so that there are multiple command buffers to correspond to the multiple framebuffers.
- [x] Change the command buffer that draws the FBO to a secondary buffer and then submit it along with the rest of the render buffers.
- [ ] Make it so that the GUIs can be rebuilt with the swap chain
