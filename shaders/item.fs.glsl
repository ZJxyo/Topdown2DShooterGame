#version 330

// Output color
layout(location = 0) out vec4 color;
uniform bool is_active;

void main()
{
    if (is_active) {
        color = vec4(0.f, 0.7f, 0.f, 1.f);
    } else {
        color = vec4(0.f, 0.3f, 0.f, 1.f);
    }
}