#version 330
layout (triangles) in;
layout (points, max_vertices = 100) out;

void main() {    
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
    EmitVertex();
    EndPrimitive();
}