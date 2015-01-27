#extension GL_ARB_texture_rectangle : enable

uniform vec2 screenSize;
uniform sampler2DRect tex0; //imagen origen

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
	
	vec2 pos = gl_FragCoord.xy;
	vec3 color = texture2DRect(tex0, pos).xyz;
	vec3 c;

	float areaPixels[9];
	vec2 readPos;
	if (pos.x > screenSize.x - 1 || pos.x < 1 ||
		pos.y > screenSize.y - 1 || pos.y < 1)
	{
		gl_FragColor = vec4(0,0,0,0);
	}
	else
	{
		// loop through the area pixels
		for(int i=-1; i<=1; i++){
			for(int j=-1; j<=1; j++){
				
				// determine where to read from in the area (not optimized)
				readPos = vec2(pos.x + i , pos.y + j);
				c = texture2DRect(tex0, readPos).xyz;
				
				float BR = (0.299 * c.r) + (.587 * c.g) + (.114 * c.b);
				
				int writePos = (j+1) * 3 + (i + 1);
				
				areaPixels[writePos] = BR;
			}
		}
		
		float dX = (areaPixels[0] + areaPixels[3] + areaPixels[6])/3.0 - (areaPixels[2] + areaPixels[5] + areaPixels[8])/3.0;
		float dY = (areaPixels[0] + areaPixels[1] + areaPixels[2])/3.0 - (areaPixels[6] + areaPixels[7] + areaPixels[8])/3.0;
		
		gl_FragColor = vec4(dX, dY, 0.0, 1.0);
		//gl_FragColor = vec4(color, 1.0);
	}
}