//#version 120
#version 330 compatibility
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect particles0; //pos
uniform sampler2DRect particles1; //vel
uniform sampler2DRect particles2; //color
uniform sampler2DRect particles3; //pos_ini
uniform sampler2DRect particles4; //TEXTURE_COORDS
uniform sampler2DRect particles5; //SIZE_LIFE_MAXDST

// Tamaño de la pantalla
uniform sampler2DRect texStaticColor; //static color

uniform float colorStatic; //true:  el color de la partícula será el color de la textura de color de la posición que ocupe la partícula

out VertexData
{
    vec3 velocidad;
    vec2 part_pos;
	float vida;
	float maxDst;
	float size;
} vertex;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;

	vec3 pos = texture2DRect(particles0, gl_TexCoord[0].st).xyz;
	vec3 vel = texture2DRect(particles1, gl_TexCoord[0].st).xyz;
	vec3 pos_i = texture2DRect(particles3, gl_TexCoord[0].st).xyz;
	float size = texture2DRect(particles5, gl_TexCoord[0].st).x;
	float vida = texture2DRect(particles5, gl_TexCoord[0].st).y;
	
	vertex.vida = vida;
	vertex.size = size;
	vertex.velocidad = vel;
	vertex.part_pos = gl_TexCoord[0].st;
	vertex.maxDst = texture2DRect(particles5, gl_TexCoord[0].st).z;
	/*
	if (colorStatic > 0.0)
		v_color = texture2DRect(particles2, pos.xy).xyzw;
	else
		v_color = texture2DRect(particles2, gl_TexCoord[0].st).xyzw;
	*/
	
	if (colorStatic > 0.0)
		gl_FrontColor = texture2DRect(texStaticColor, pos.xy).xyzw;
	else
		gl_FrontColor = texture2DRect(particles2, gl_TexCoord[0].st).xyzw;
	
	//gl_FrontColor = vec4(texture2DRect(particles2, gl_TexCoord[0].st).xyz, 1.0);
	//gl_FrontColor = vec4(1.0,0.0,0.0,1.0);
	//float v = vel.x * vel.y;
	//float vn = 0.5 + (v / 100.0);
	//if ( v > 50.0)
	//	gl_FrontColor = vec4(0.5 * vn, vn, 0.5 * vn,1.0);
	//else
		//gl_FrontColor = gl_Color;
		
	//gl_FrontColor = vec4(color,1.0);
	
	//gl_FrontColor = col;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(texture2DRect(particles0, gl_TexCoord[0].st).xyz, 1.0);
}
