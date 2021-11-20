#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float darken_screen_factor;

in vec2 texcoord;

layout(location = 0) out vec4 color;

//
void main()
{
    vec2 resolution = vec2(1080, 720);

    float offset = (time- floor(time))/time;
	float CurrentTime = (time)*(offset);    
    
	vec3 WaveParams = vec3(10.0, 0.8, 0.1 ); 
    
    vec2 WaveCentre = vec2(0.5, 0.5);
   
	vec2 coord = texcoord;
	float Dist = distance(coord, WaveCentre);
    
	color = texture(screen_texture, coord);
    if (Dist <= (CurrentTime + WaveParams.z) && Dist >= (CurrentTime - WaveParams.z)) 
	{
        //The pixel offset distance based on the input parameters
		float Diff = (Dist - CurrentTime); 
		float ScaleDiff = (1.0 - pow(abs(Diff * WaveParams.x), WaveParams.y)); 
		float DiffTime = (Diff  * ScaleDiff);
        
        //The direction of the distortion
		vec2 Diffcoord = normalize(coord - WaveCentre);         
        
        //Perform the distortion and reduce the effect over time
		coord += ((Diffcoord * DiffTime) / (CurrentTime * Dist * 40.0));
		color = texture(screen_texture, coord);
        
        //Blow out the color and reduce the effect over time
		color += (color * ScaleDiff) / (CurrentTime * Dist * 40.0);
	}
}