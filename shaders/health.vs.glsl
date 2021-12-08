#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int health;
uniform int curframe;

void main()
{
	float healthp = float(health) / 1000;
	texcoord.x = in_texcoord.x;
	texcoord.y = in_texcoord.y;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	pos.x = pos.x * healthp; 
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}