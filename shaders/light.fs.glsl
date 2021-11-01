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
    color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
    if (team_color == 1)
    {
//        color.xyz = vec3(color.x * 1.0,color.y * 1.0,color.z * 1.0);
//        color.xyz += 0.1 * 0.8 * vec3(1.0, 1.0, 1.0);
        color.xyz = texture(sampler0, vec2(texcoord.x, texcoord.y)).rgb * 3;
    }
}
//#version 330 core
//
//in vec2 texcoord;
//
//uniform sampler2D   tex;
//
//void main() {
//    gl_FragColor = /*vec4(1 - texCoord.xyx, 1);*/texture(tex, texCoord);
//}