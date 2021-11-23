#version 330

// From vertex shader
in vec2 texcoord;

uniform float alpha;
uniform vec3 color;

// Application data
layout(location = 0) out vec4 out_color;

void main()
{
	if (length(texcoord - vec2(0.5f, 0.5f)) > 0.5f) {
		discard;
	} else {
		out_color = vec4(color, alpha);
	}
}