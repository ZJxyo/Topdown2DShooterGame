#version 330

// Input attributes
in vec3 in_position;

void main()
{
    gl_Position = vec4(in_position.xy, in_position.z, 1.0);
}