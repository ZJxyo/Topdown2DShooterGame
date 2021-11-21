#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform vec2 shockwave_position;
uniform float dimming_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

// adapted from Darthmarshie's work on shadertoy.com
void main()
{
	color = texture(screen_texture, texcoord);
	color.xy *= dimming_factor;

	if (time > 10.f) {
		return;
	}

	float CurrentTime = time;    
    
	vec3 WaveParams = vec3(10.0, 0.8, 0.1 ); 
       
	vec2 coord = texcoord;
	float Dist = distance(coord, shockwave_position);
    
    if (Dist <= (CurrentTime + WaveParams.z) && Dist >= (CurrentTime - WaveParams.z)) 
	{
        //The pixel offset distance based on the input parameters
		float Diff = (Dist - CurrentTime); 
		float ScaleDiff = (1.0 - pow(abs(Diff * WaveParams.x), WaveParams.y)); 
		float DiffTime = (Diff  * ScaleDiff);
        
        //The direction of the distortion
		vec2 Diffcoord = normalize(coord - shockwave_position);         
        
        //Perform the distortion and reduce the effect over time
		coord += ((Diffcoord * DiffTime) / (CurrentTime * Dist * 40.f));
		color = texture(screen_texture, coord);
        color.xy *= dimming_factor;

        //Blow out the color and reduce the effect over time
		color += (color * ScaleDiff) / (CurrentTime * Dist * 100.f + 0.3);
	}
}