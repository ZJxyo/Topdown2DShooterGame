#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int team_color;

// Output color
layout(location = 0) out  vec4 color;

void main()
{
	color = vec4(0.0,0.5,0.0, 0.2);
	
}
