#version 330

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;
uniform int repeatx;
uniform int repeaty;

void main()
{
    texcoord.x = repeatx * in_texcoord.x;
    texcoord.y = repeaty * in_texcoord.y;
    vec3 pos = projection * transform * vec3(in_position.xy, 1.0);
    gl_Position = vec4(pos.xy, in_position.z, 1.0);
}

// Vertex shader for a point-light (local) source, with computation
// done in the fragment shader.
//#version 330 core
//
//// uniform mat4    uPMatrix;
//// uniform mat4    uMVMatrix;
//
//layout (location = 0) in vec2 aPosition;
//layout (location = 2) in vec2 aTexCoord;
//
//out vec2    texCoord;
//
//void main() {
//    vec4 pos = vec4(aPosition, 1.0F, 1.0F);
//    // vec4 mvPos = uMVMatrix * pos;
//    // gl_Position = uPMatrix * mvPos;
//    gl_Position = pos;
//
//    texCoord = aTexCoord;
//}