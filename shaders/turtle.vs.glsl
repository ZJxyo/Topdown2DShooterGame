#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int repeatx;
uniform int repeaty;

void main()
{
	texcoord.x = repeatx * in_texcoord.x;
	texcoord.y = repeaty * in_texcoord.y;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}