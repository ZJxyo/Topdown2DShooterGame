// internal
#include "render_system.hpp"
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection, RenderRequest &render_request, vec2 scaling = {1, 1})
{
	if (registry.enemies.has(entity)) {
		if (!(registry.enemies.get(entity).is_visible))
			return;
	}
	Motion &motion = registry.motions.get(entity);
	// Transformation code, see Rendering and Transformation in the template
	// specification for more info Incrementally updates transformation matrix,
	// thus ORDER IS IMPORTANT
	Entity player = registry.players.entities[0];
	vec2 pos = registry.motions.get(player).position;

	Transform transform;
	transform.translate(vec2(window_width_px / 2 - pos.x, window_height_px / 2 - pos.y)); // translate camera to player
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	transform.scale(motion.scale);
	transform.scale(scaling);
	// !!! TODO A1: add rotation to the chain of transformations, mind the order
	// of transformations

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		GLuint texture_id =
			texture_gl_handles[(GLuint)render_request.used_texture];

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		gl_has_errors();
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}
	else if (render_request.used_texture == TEXTURE_ASSET_ID::PLAYER || render_request.used_texture == TEXTURE_ASSET_ID::FEET)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
							  sizeof(TexturedVertex), (void *)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void *)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();


		GLint sprite_width_loc = glGetUniformLocation(program, "spriteWidth");
		GLint cur_frame_loc = glGetUniformLocation(program, "curframe");
		GLint frames_loc = glGetUniformLocation(program, "frames");
		Animate &a = registry.animates.get(entity);
		if (render_request.used_texture == TEXTURE_ASSET_ID::FEET){
			glUniform1i(sprite_width_loc, a.feet_width);
			glUniform1i(cur_frame_loc, a.sprite_frame);
			glUniform1i(frames_loc, a.feet_frames);
		} else {
			glUniform1i(sprite_width_loc, a.player_width);
			glUniform1i(cur_frame_loc, a.sprite_frame);
			glUniform1i(frames_loc, a.player_frames);
		}
		gl_has_errors();

		GLint light_up_uloc = glGetUniformLocation(program, "team_color");
		assert(light_up_uloc >= 0);
		// lightup red for enemy
		if (registry.enemies.has(entity))
		{
			glUniform1i(light_up_uloc, 1);
		}
		else
		{
			glUniform1i(light_up_uloc, 0);
		}

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)render_request.used_texture];
		
		gl_has_errors();
		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);

	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED && registry.floorRenderRequests.has(entity))
	{
		GLint tex_uloc = glGetUniformLocation(currProgram, "repeatx");
		glUniform1i(tex_uloc, motion.scale.x/100);
		gl_has_errors();

		GLint tex_uloc2 = glGetUniformLocation(currProgram, "repeaty");
		glUniform1i(tex_uloc2, motion.scale.y/100);
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED && registry.walls.has(entity))
	{
		GLint tex_uloc = glGetUniformLocation(currProgram, "repeatx");
		glUniform1i(tex_uloc, motion.scale.x / 60);
		gl_has_errors();

		GLint tex_uloc2 = glGetUniformLocation(currProgram, "repeaty");
		glUniform1i(tex_uloc2, motion.scale.y / 30);
		gl_has_errors();
	}
	else
	{
		GLint tex_uloc = glGetUniformLocation(currProgram, "repeatx");
		glUniform1i(tex_uloc, 1);
		gl_has_errors();
		GLint tex_uloc2 = glGetUniformLocation(currProgram, "repeaty");
		glUniform1i(tex_uloc2, 1);
		gl_has_errors();
		if (registry.players.has(entity) && render_request.used_texture == TEXTURE_ASSET_ID::PLAYER)
		{
			GLint c_uloc = glGetUniformLocation(currProgram, "team_color");
			glUniform1i(c_uloc, 1);
			gl_has_errors();
		}
	}

	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawTexturedInstances(std::vector<Entity>& entities,
	const mat3& projection, RenderRequest& request) {

	const GLuint program = effects[(GLuint)request.used_effect];
	glUseProgram(program);
	gl_has_errors();

	const GLuint vbo = vertex_buffers[(GLuint)request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)request.used_geometry];
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), 0);
	glEnableVertexAttribArray(0);
	gl_has_errors();

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
	glEnableVertexAttribArray(1);
	gl_has_errors();

	glActiveTexture(GL_TEXTURE0);
	gl_has_errors();

	GLuint texture_id = texture_gl_handles[(GLuint)request.used_texture];

	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//gl_has_errors();
	glBindTexture(GL_TEXTURE_2D, texture_id);
	gl_has_errors();

	vec2 player_postion = registry.motions.get(registry.players.entities[0]).position;
	vec2 offset = vec2(window_width_px / 2.f - player_postion.x, window_height_px / 2.f - player_postion.y);

	std::vector<glm::mat3> transforms;

	for (auto e : entities) {
		Motion& motion = registry.motions.get(e);
		Transform transform;
		transform.translate(motion.position + offset);
		transform.rotate(motion.angle);
		transform.scale(motion.scale);
		transforms.push_back(transform.mat);
	}

	glBindBuffer(GL_ARRAY_BUFFER, transform_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat3) * transforms.size(), transforms.data(), GL_STREAM_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 3, 0);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 3, (void*)sizeof(vec3));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) * 3, (void*)(sizeof(vec3) * 2));
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	gl_has_errors();

	GLint projection_loc = glGetUniformLocation(program, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);

	glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr, entities.size());
	gl_has_errors();
}

void RenderSystem::drawParticles(ParticleSource ps, mat3 projection) {
	const GLuint program = effects[(GLuint)EFFECT_ASSET_ID::PARTICLE];
	glUseProgram(program);
	gl_has_errors();

	const GLuint vbo = vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE];
	const GLuint ibo = index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SPRITE];
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), 0);
	glEnableVertexAttribArray(0);
	gl_has_errors();

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));
	glEnableVertexAttribArray(1);
	gl_has_errors();

	vec2 player_postion = registry.motions.get(registry.players.entities[0]).position;
	vec2 offset = vec2(window_width_px / 2.f - player_postion.x, window_height_px / 2.f - player_postion.y);

	std::vector<vec2> positions;
	for (int i = 0; i < ps.size; i++) {
		positions.push_back(ps.positions[i] + offset);
	}

	glBindBuffer(GL_ARRAY_BUFFER, transform_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * positions.size(), positions.data(), GL_STREAM_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);
	gl_has_errors();

	GLint radius_loc = glGetUniformLocation(program, "radius");
	glUniform1f(radius_loc, ps.radius);
	gl_has_errors();

	GLint alpha_loc = glGetUniformLocation(program, "alpha");
	glUniform1f(alpha_loc, ps.alpha);
	gl_has_errors();

	GLint color_loc = glGetUniformLocation(program, "color");
	glUniform3f(color_loc, ps.color.x, ps.color.y, ps.color.z);
	gl_has_errors();

	GLint projection_loc = glGetUniformLocation(program, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	gl_has_errors();

	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);

	glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr, ps.size);
	gl_has_errors();
}

void RenderSystem::drawCustomMesh(Entity entity, mat3& projection, RenderRequest& render_request) {
	const GLuint program = effects[(GLuint)render_request.used_effect];
	glUseProgram(program);
	gl_has_errors();

	CustomMesh& cm = registry.customMeshes.get(entity);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * cm.vertices.size(), cm.vertices.data(), GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * cm.indices.size(), cm.indices.data(), GL_STREAM_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	glEnableVertexAttribArray(0);

	GLint color_uloc = glGetUniformLocation(program, "color");
	glUniform3fv(color_uloc, 1, (float*)&cm.color);

	GLint projection_loc = glGetUniformLocation(program, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);

	vec2 player_postion = registry.motions.get(registry.players.entities[0]).position;
	vec2 offset = vec2(window_width_px / 2.f - player_postion.x, window_height_px / 2.f - player_postion.y);
	Transform transform;
	transform.translate(offset);

	GLint transform_loc = glGetUniformLocation(program, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float*)&transform.mat);

	glDrawElements(GL_TRIANGLES, cm.indices.size(), GL_UNSIGNED_INT, nullptr);
}

// draw the intermediate texture to the screen, with some distortion to simulate
// water
void RenderSystem::drawToScreen()
{
	const GLuint water_program = effects[(GLuint)EFFECT_ASSET_ID::WATER];

	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(water_program);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 0, 0, 1.0);
	glClearDepth(1.f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();
	glEnable(GL_STENCIL_TEST);

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();

	GLint time_uloc = glGetUniformLocation(water_program, "time");
	GLint shockwave_pos_uloc = glGetUniformLocation(water_program, "shockwave_pos");
	GLint darken_factor_uloc = glGetUniformLocation(water_program, "darken_factor");

	// send time and shockwave first
	if (registry.shockwaveSource.size() > 0) {
		ShockwaveSource& sws = registry.shockwaveSource.components[0];
		vec2 pos = registry.motions.get(registry.players.entities[0]).position;
		glUniform1f(time_uloc, sws.time_elapsed);
		glUniform2f(shockwave_pos_uloc, (sws.pos.x - pos.x) / window_width_px + 0.5f, -(sws.pos.y - pos.y) / window_height_px + 0.5f);
	}
	else {
		glUniform1f(time_uloc, -1.f);
	}

	// visibility polygon
	if (registry.lightSources.size() > 0) {
		CustomMesh& ls = registry.lightSources.components[0];
		glBindBuffer(GL_ARRAY_BUFFER, vertices_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * ls.vertices.size(), ls.vertices.data(), GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ls.indices.size(), ls.indices.data(), GL_STREAM_DRAW);
		gl_has_errors();

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

		glUniform1f(darken_factor_uloc, 1.f);

		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
		glDrawElements(GL_TRIANGLES, ls.indices.size(), GL_UNSIGNED_INT, nullptr);
		gl_has_errors();
	}

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); 
	// Note, GL_ELEMENT_ARRAY_BUFFER associates
	// indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();

	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	glUniform1f(darken_factor_uloc, 0.6f);

	gl_has_errors();

	// Draw
	glStencilFunc(GL_NOTEQUAL, 1, 0xff);
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_DEPTH_TEST);
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(1.f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	// Draw all textured meshes that have a position and size component

	for (Entity entity : registry.floorRenderRequests.entities)
	{
		if (!registry.motions.has(entity))
			continue;

		RenderRequest &render_request = registry.floorRenderRequests.get(entity);
		drawTexturedMesh(entity, projection_2D, render_request);
	}

	for (Entity entity : registry.renderRequests2.entities)
	{
		if (!registry.motions.has(entity))
			continue;
		RenderRequest &render_request2 = registry.renderRequests2.get(entity);

		drawTexturedMesh(entity, projection_2D, render_request2, {0.7, 0.7});
	}

	for (int i = 0; i < registry.renderRequests.size(); i++)
	{
		Entity e = registry.renderRequests.entities[i];
		if (!registry.motions.has(e))
			continue;

		drawTexturedMesh(e, projection_2D, registry.renderRequests.components[i]);
	}

	for (int i = 0; i < registry.customMeshRenderRequests.size(); i++) {
		drawCustomMesh(registry.customMeshRenderRequests.entities[i], projection_2D, registry.customMeshRenderRequests.components[i]);
	}

	if (registry.bulletsRenderRequests.entities.size() > 0) {
		drawTexturedInstances(registry.bulletsRenderRequests.entities, projection_2D, registry.bulletsRenderRequests.components[0]);
	}

	for (ParticleSource ps : registry.particleSources.components) {
		drawParticles(ps, projection_2D);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	gl_has_errors();
	float right = (float)window_width_px;
	float bottom = (float)window_height_px;

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}
