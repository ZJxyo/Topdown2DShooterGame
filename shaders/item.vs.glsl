#version 330

// Input attributes
layout (location = 0) in vec3 in_position;
uniform mat3 projection;

void main()
{
    gl_Position = vec4(in_position.xy, 1.f, 1.f);
}