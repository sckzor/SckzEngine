# SckzEngine

#### A Vulkan games engine by Sckzor

All code in this repository is copyright Charles Sands 2021. All rights reserved!

## To do

#### Things to do in the current branch before it is merged to master

Each model should have 4 different textures:

- A color texture
- A normal map
- A specular map
- A texture full of extra stuff that has it's function determined by the shader.

Each one of these texures does not nessisarially need to be loaded to the shader and if
any one of them is missing (except for the color texture) then a default of a 1x1 black
texture will be uploaded into the shader by the sampler. The shader will know to ignore
the 1x1 black texture and use the default normals/specular/extra map.

The attachments added to the sampler will always start with a number of UBOs on the vertex
buffer, followed by a number of samplers on the fragment shader, then a number of UBOs on
the fragment shader. It will ALWAYS go in this order.
