#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D tex;

void main()
{
	gl_FragColor = texture(tex, texcoord);
}
