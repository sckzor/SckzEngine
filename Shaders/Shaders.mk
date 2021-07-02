RESOURCES_DIR = ../Resources

Shaders:SimpleShaders BlueShaders FBOShaders GUIShaders ParticleShaders

SimpleShaders: $(RESOURCES_DIR)/simple_vertex.spv $(RESOURCES_DIR)/simple_fragment.spv

$(RESOURCES_DIR)/simple_fragment.spv: simple_shader.frag
	glslc simple_shader.frag -o $(RESOURCES_DIR)/simple_fragment.spv

$(RESOURCES_DIR)/simple_vertex.spv: simple_shader.vert
	glslc simple_shader.vert -o $(RESOURCES_DIR)/simple_vertex.spv


FBOShaders:  $(RESOURCES_DIR)/fbo_vertex_normal.spv $(RESOURCES_DIR)/fbo_fragment_combine_shader.spv $(RESOURCES_DIR)/fbo_fragment_normal.spv $(RESOURCES_DIR)/fbo_fragment_outline.spv $(RESOURCES_DIR)/fbo_fragment_invert.spv

$(RESOURCES_DIR)/fbo_vertex_normal.spv: fbo_shader.vert
	glslc fbo_shader.vert -o $(RESOURCES_DIR)/fbo_vertex_normal.spv

$(RESOURCES_DIR)/fbo_fragment_normal.spv: fbo_shader.frag
	glslc fbo_shader.frag -o $(RESOURCES_DIR)/fbo_fragment_normal.spv

$(RESOURCES_DIR)/fbo_fragment_outline.spv: fbo_outline.frag
	glslc fbo_outline.frag -o $(RESOURCES_DIR)/fbo_fragment_outline.spv

$(RESOURCES_DIR)/fbo_fragment_invert.spv: fbo_invert.frag
	glslc fbo_invert.frag -o $(RESOURCES_DIR)/fbo_fragment_invert.spv

$(RESOURCES_DIR)/fbo_fragment_combine_shader.spv: fbo_combine_shader.frag
	glslc fbo_combine_shader.frag -o $(RESOURCES_DIR)/fbo_fragment_combine_shader.spv


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


BlueShaders: $(RESOURCES_DIR)/blue_fragment.spv

$(RESOURCES_DIR)/blue_fragment.spv: blue_shader.frag
	glslc blue_shader.frag -o $(RESOURCES_DIR)/blue_fragment.spv


clean:
	rm -f $(RESOURCES_DIR)/*.spv