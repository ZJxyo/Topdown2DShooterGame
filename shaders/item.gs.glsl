#version 330
layout (points) in;
layout (triangle_strip, max_vertices = 8) out;

uniform int type;
uniform mat3 projection;

void main() {

    vec2 center = gl_in[0].gl_Position.xy;
    // cross
    if (type == 0) {
        float offset1 = 10.f;
        float offset2 = 30.f;
        gl_Position = vec4(projection * vec3(center.x - offset1, center.y + offset2, 1.f), 1.f);
        EmitVertex();
        gl_Position = vec4(projection * vec3(center.x + offset1, center.y + offset2, 1.f), 1.f);
        EmitVertex();
        gl_Position = vec4(projection * vec3(center.x - offset1, center.y - offset2, 1.f), 1.f);
        EmitVertex();
        gl_Position = vec4(projection * vec3(center.x + offset1, center.y - offset2, 1.f), 1.f);
        EmitVertex();
        EndPrimitive();

        gl_Position = vec4(projection * vec3(center.x - offset2, center.y + offset1, 1.f), 1.f);
        EmitVertex();
        gl_Position = vec4(projection * vec3(center.x + offset2, center.y + offset1, 1.f), 1.f);
        EmitVertex();
        gl_Position = vec4(projection * vec3(center.x - offset2, center.y - offset1, 1.f), 1.f);
        EmitVertex();
        gl_Position = vec4(projection * vec3(center.x + offset2, center.y - offset1, 1.f), 1.f);
        EmitVertex();
        EndPrimitive();

    } else if (type == 1) {
        gl_Position = vec4(center, 1.f, 1.f);
        EmitVertex();
        EndPrimitive();
    } else {
        gl_Position = vec4(center, 1.f, 1.f);
        EmitVertex();
        EndPrimitive();
    }
}