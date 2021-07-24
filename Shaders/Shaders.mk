RESOURCES_DIR = ../Resources

Shaders:SimpleShaders BlueShaders FBOShaders GUIShaders ParticleShaders SkyBoxShaders CubemapRenderShaders CubemapRenderCubeShaders



SimpleShaders: $(RESOURCES_DIR)/simple_vertex.spv $(RESOURCES_DIR)/simple_fragment.spv

$(RESOURCES_DIR)/simple_fragment.spv: simple_shader.frag
	glslc simple_shader.frag -o $(RESOURCES_DIR)/simple_fragment.spv

$(RESOURCES_DIR)/simple_vertex.spv: simple_shader.vert
	glslc simple_shader.vert -o $(RESOURCES_DIR)/simple_vertex.spv



FBOShaders:  $(RESOURCES_DIR)/fbo_fragment_gaussian_horizontal.spv $(RESOURCES_DIR)/fbo_fragment_gaussian_vertical.spv $(RESOURCES_DIR)/fbo_fragment_bright.spv $(RESOURCES_DIR)/fbo_vertex_normal.spv $(RESOURCES_DIR)/fbo_fragment_combine_additive.spv $(RESOURCES_DIR)/fbo_fragment_normal.spv $(RESOURCES_DIR)/fbo_fragment_outline.spv $(RESOURCES_DIR)/fbo_fragment_invert.spv

$(RESOURCES_DIR)/fbo_vertex_normal.spv: fbo_shader.vert
	glslc fbo_shader.vert -o $(RESOURCES_DIR)/fbo_vertex_normal.spv

$(RESOURCES_DIR)/fbo_fragment_normal.spv: fbo_shader.frag
	glslc fbo_shader.frag -o $(RESOURCES_DIR)/fbo_fragment_normal.spv

$(RESOURCES_DIR)/fbo_fragment_outline.spv: fbo_outline.frag
	glslc fbo_outline.frag -o $(RESOURCES_DIR)/fbo_fragment_outline.spv

$(RESOURCES_DIR)/fbo_fragment_invert.spv: fbo_invert.frag
	glslc fbo_invert.frag -o $(RESOURCES_DIR)/fbo_fragment_invert.spv

$(RESOURCES_DIR)/fbo_fragment_combine_additive.spv: fbo_combine_additive.frag
	glslc fbo_combine_additive.frag -o $(RESOURCES_DIR)/fbo_fragment_combine_additive.spv

$(RESOURCES_DIR)/fbo_fragment_bright.spv: fbo_bright.frag
	glslc fbo_bright.frag -o $(RESOURCES_DIR)/fbo_fragment_bright.spv

$(RESOURCES_DIR)/fbo_fragment_gaussian_vertical.spv: fbo_gaussian_vertical.frag
	glslc fbo_gaussian_vertical.frag -o $(RESOURCES_DIR)/fbo_fragment_gaussian_vertical.spv

$(RESOURCES_DIR)/fbo_fragment_gaussian_horizontal.spv: fbo_gaussian_horizontal.frag
	glslc fbo_gaussian_horizontal.frag -o $(RESOURCES_DIR)/fbo_fragment_gaussian_horizontal.spv



ParticleShaders: $(RESOURCES_DIR)/particle_fragment.spv $(RESOURCES_DIR)/particle_vertex.spv 

$(RESOURCES_DIR)/particle_vertex.spv: particle_shader.vert
	glslc particle_shader.vert -o $(RESOURCES_DIR)/particle_vertex.spv

$(RESOURCES_DIR)/particle_fragment.spv: particle_shader.frag
	glslc particle_shader.frag -o $(RESOURCES_DIR)/particle_fragment.spv



GUIShaders: $(RESOURCES_DIR)/gui_fragment_normal.spv $(RESOURCES_DIR)/gui_vertex_normal.spv

$(RESOURCES_DIR)/gui_vertex_normal.spv: gui_shader.vert
	glslc gui_shader.vert -o $(RESOURCES_DIR)/gui_vertex_normal.spv

$(RESOURCES_DIR)/gui_fragment_normal.spv: gui_shader.frag
	glslc gui_shader.frag -o $(RESOURCES_DIR)/gui_fragment_normal.spv



SkyBoxShaders: $(RESOURCES_DIR)/skybox_vertex.spv $(RESOURCES_DIR)/skybox_fragment.spv

$(RESOURCES_DIR)/skybox_vertex.spv: skybox.vert
	glslc skybox.vert -o $(RESOURCES_DIR)/skybox_vertex.spv

$(RESOURCES_DIR)/skybox_fragment.spv: skybox.frag
	glslc skybox.frag -o $(RESOURCES_DIR)/skybox_fragment.spv



CubemapRenderShaders: $(RESOURCES_DIR)/cubemap_render_vertex.spv $(RESOURCES_DIR)/cubemap_render_fragment.spv

$(RESOURCES_DIR)/cubemap_render_vertex.spv: cubemap_render.vert
	glslc cubemap_render.vert -o $(RESOURCES_DIR)/cubemap_render_vertex.spv

$(RESOURCES_DIR)/cubemap_render_fragment.spv: cubemap_render.frag
	glslc cubemap_render.frag -o $(RESOURCES_DIR)/cubemap_render_fragment.spv

CubemapRenderCubeShaders: $(RESOURCES_DIR)/cubemap_render_cube_vertex.spv $(RESOURCES_DIR)/cubemap_render_cube_fragment.spv

$(RESOURCES_DIR)/cubemap_render_cube_vertex.spv: cubemap_render_cube.vert
	glslc cubemap_render_cube.vert -o $(RESOURCES_DIR)/cubemap_render_cube_vertex.spv

$(RESOURCES_DIR)/cubemap_render_cube_fragment.spv: cubemap_render_cube.frag
	glslc cubemap_render_cube.frag -o $(RESOURCES_DIR)/cubemap_render_cube_fragment.spv



BlueShaders: $(RESOURCES_DIR)/blue_fragment.spv

$(RESOURCES_DIR)/blue_fragment.spv: blue_shader.frag
	glslc blue_shader.frag -o $(RESOURCES_DIR)/blue_fragment.spv



clean:
	rm -f $(RESOURCES_DIR)/*.spv