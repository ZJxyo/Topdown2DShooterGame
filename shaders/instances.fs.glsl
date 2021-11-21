#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D tex;
layout(location = 0) out vec4 color;

void main()
{
	color = texture(tex, texcoord);
}
