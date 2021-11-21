#version 330

// Input attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texcoord;
layout(location = 2) in vec2 position;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform float radius;
uniform mat3 projection;

void main()
{
	texcoord = in_texcoord;

	vec3 pos = projection * vec3(in_position.xy * 2 * radius + position, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}