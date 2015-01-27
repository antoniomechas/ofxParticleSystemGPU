#version 330 compatibility
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect multiTex;
uniform sampler2DRect sparkTex;
uniform float factorVelocidadColor;	// si es mayor que 0, le aplica el factor
uniform float useParticleTexture;	// si le pone textura a la particula
uniform float blackWhenStill;		// si la velocidad es casi cero, no dibuja la particula
uniform float minVelocidad;			// por debajo de esta velocidad, se considera que la particula no tiene velocidad
uniform float useVida;				// Tiene en cuenta la vida de la partícula (si es 0, no la dibujará)
uniform int   useMultiTexture;

in float f_velocidad;			// tamaño del vector velocidad
in float f_vida;

void main() {
    
	if (f_vida <= 0 && useVida > 0.0)
		discard;
	
	vec2 st = gl_TexCoord[0].st;
	
	float intensidad = 0.0;
	
	if (factorVelocidadColor > 0.0)
		intensidad = f_velocidad / factorVelocidadColor;
	
	vec4 color;
	if (useParticleTexture > 0.0)
	{
		if (useMultiTexture > 0)
			color = texture2DRect(multiTex, st);
		else
			color = texture2DRect(sparkTex, st);
	}
	else
	{
		color = vec4(1.0,1.0,1.0,1.0);
	}
	
	if (blackWhenStill > 0 && f_velocidad < minVelocidad)
		gl_FragColor = vec4(0, 0, 0, 0);
	else
		gl_FragColor = vec4(color.x * gl_Color.x + intensidad, color.y * gl_Color.y + intensidad, color.z * gl_Color.z + intensidad, color.w);
	//gl_FragColor = vec4(color.x * gl_Color.x, color.y * gl_Color.y, color.z * gl_Color.z, color.w);


	//gl_FragColor = vec4(f_vida / 300.0, f_vida / 300.0, f_vida / 300.0, 1.0);
	
	//gl_FragColor = vec4(f_color.x, f_color.y, f_color.z, 1.0);
	//gl_FragColor = vec4(f_color.x, f_color.y, f_color.z, 1.0);
	//gl_FragColor = vec4(color.x, color.y, color.z, color.w);
    //gl_FragColor = gl_Color;

	//gl_FragColor = vec4(f_color.x, f_color.y, f_color.z, 1.0);
	//gl_FragColor = vec4(0.5, 0.5, f_color.z, 1.0);
	//gl_FragColor = gl_Color;
}
