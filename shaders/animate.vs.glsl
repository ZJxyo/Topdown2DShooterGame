#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int frames;
uniform int curframe;
uniform int spriteWidth;

void main()
{
	texcoord.x = in_texcoord.x / frames;
	texcoord.y = in_texcoord.y;
	float frac = float(curframe) / frames;
	texcoord.x += frac;
	vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
	gl_Position = vec4(pos.xy, in_position.z, 1.0);
}