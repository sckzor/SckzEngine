RESOURCES_DIR = ../Resources

Shaders:SimpleShaders BlueShaders

SimpleShaders: $(RESOURCES_DIR)/simple_vertex.spv $(RESOURCES_DIR)/simple_fragment.spv

$(RESOURCES_DIR)/simple_fragment.spv: simple_shader.frag
	glslc simple_shader.frag -o $(RESOURCES_DIR)/simple_fragment.spv

$(RESOURCES_DIR)/simple_vertex.spv: simple_shader.vert
	glslc simple_shader.vert -o $(RESOURCES_DIR)/simple_vertex.spv

SimpleShaders: $(RESOURCES_DIR)/fbo_vertex.spv $(RESOURCES_DIR)/fbo_fragment.spv

$(RESOURCES_DIR)/fbo_fragment.spv: fbo_shader.frag
	glslc fbo_shader.frag -o $(RESOURCES_DIR)/fbo_fragment.spv

$(RESOURCES_DIR)/fbo_vertex.spv: fbo_shader.vert
	glslc fbo_shader.vert -o $(RESOURCES_DIR)/fbo_vertex.spv


BlueShaders: $(RESOURCES_DIR)/blue_fragment.spv

$(RESOURCES_DIR)/blue_fragment.spv: blue_shader.frag
	glslc blue_shader.frag -o $(RESOURCES_DIR)/blue_fragment.spv


clean:
	rm -f $(RESOURCES_DIR)/*.spv