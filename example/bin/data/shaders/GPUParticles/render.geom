#version 330 compatibility
#extension GL_EXT_geometry_shader4 : enable
#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect particles0; //pos
uniform sampler2DRect particles1; //vel
uniform sampler2DRect particles2; //color
uniform sampler2DRect particles3; //pos_ini
uniform sampler2DRect particles4; //TEXTURE_COORDS
uniform sampler2DRect particles5; //SIZE_LIFE_MAXDST

//uniform float size;
uniform float imgWidth, imgHeight;
uniform int   useMultiTexture;

uniform float enlargeByVelocity; // >0: cambia el tamaño de la textura según la velocidad de la particula
uniform float sizeByVelocity; // >0: cambia el tamaño de la particula según la velocidad de la particula
uniform float originStatic;		// dibuja la particula desde la posición incial hasta donde se encuentre
uniform float useMaxDst;		// >0 tendrá en cuenta la distancia maxima desde el origen de la particula

in VertexData
{
    vec3 velocidad;
    vec2 part_pos;
	float vida;
	float maxDst;
	float size;
} vertex[];

out float f_velocidad;
out float f_vida;

void main(void){

    // For each vertex moved to the right position on the vertex shader
    // it makes 6 more vertex that makes 2 GL_TRIANGLE_STRIP
    // that´s going to be the frame for the pixels of the sparkImg texture
    //
	vec4 p1,p2,p3,p4;
	vec3 right, dir;
	vec4 pos_in;
	vec3 up = vec3(0, 0, 1);	// arbitrary up vector
	float vel2;
	float size = vertex[0].size;
	
	float sizeFinal = size;
	
	if (sizeByVelocity > 0)
	{
		float velo = length(vertex[0].velocidad);
		sizeFinal = size * velo * sizeByVelocity;
	}
	
	vec4 pos_inicial = gl_ModelViewProjectionMatrix * vec4(texture2DRect(particles3, vertex[0].part_pos).xyz, 1.0);
	if (originStatic > 0)
	{
		pos_in = pos_inicial;
		vec3 vel = gl_PositionIn[0].xyz - pos_in.xyz;

		if (useMaxDst > 0 && vertex[0].maxDst > 0)
		{
			if (length(vel) > vertex[0].maxDst) 
			{
				vec3 p = normalize(vel) * vertex[0].maxDst;
				vel = p;
			}
		}

		vel2 = length(vel);
		dir = normalize(vel);			// normalized direction vector from p0 to p1
		right = normalize(cross(dir, up));	// right vector
		right *= sizeFinal;
		dir *= length(vel);// * 0.5;
	}
	else
	{
		//Si MaxDst > 0, tiene que cambiar la posición para que no se aleje mas de MaxDst de la posición inicial de la partícula
		pos_in = gl_PositionIn[0];
		if (useMaxDst > 0 && vertex[0].maxDst > 0)
		{
			vec3 d = gl_PositionIn[0].xyz - pos_inicial.xyz;
			if (length(d) > vertex[0].maxDst) 
			{
				vec3 p = normalize(d) * vertex[0].maxDst;
				pos_in = pos_inicial + vec4(p, 1.0);
				//pos_in = gl_PositionIn[0];
			}
		}
			
		vec3 vel = vertex[0].velocidad;
		vel.y = vel.y * -1.0;
		vel2 = abs(length(vel));

		dir = normalize(vel);			
		right = normalize(cross(dir, up));	
		right *= sizeFinal;
		dir *= vel2 * enlargeByVelocity;// * 0.5;
	}

	vec4 ancho = vec4(right, 0.0);
	vec4 alto = vec4(dir, 0.0);
	
	
	if ((enlargeByVelocity > 0 || originStatic > 0) && (length(alto) > length(ancho)))
	{
		p1 = pos_in - ancho + alto;
		p2	= pos_in +ancho + alto;
		p3 = pos_in + ancho;
		p4 = pos_in - ancho;
	}
	else
	{
		p1 = pos_in +vec4(-sizeFinal,-sizeFinal,0.0,0.0);
		p2 = pos_in +vec4(sizeFinal,-sizeFinal,0.0,0.0);
		p3 = pos_in +vec4(sizeFinal,sizeFinal,0.0,0.0);
		p4 = pos_in +vec4(-sizeFinal,sizeFinal,0.0,0.0);
	}
	vec2 t1,t2,t3,t4;
	if (useMultiTexture > 0)
	{
		vec4 multiTex = texture2DRect(particles4, vertex[0].part_pos);
		t1 = vec2(multiTex.x, multiTex.y);
		t2 = vec2(multiTex.x + multiTex.z, multiTex.y);
		t3 = vec2(multiTex.x + multiTex.z, multiTex.y + multiTex.w);
		t4 = vec2(multiTex.x, multiTex.y + multiTex.w);
	}
	else
	{
		t1 = vec2(0,0);
		t2 = vec2(imgWidth,0);
		t3 = vec2(imgWidth,imgHeight);
		t4 = vec2(0,imgHeight);
	}
	
	gl_Position =  ( p1);
	f_velocidad = vel2;
	f_vida = vertex[0].vida;
	gl_TexCoord[0].x = t1.x;
	gl_TexCoord[0].y = t1.y;
	gl_FrontColor = gl_FrontColorIn[0];
	EmitVertex();
	
	gl_Position =  (p2);
	f_velocidad = vel2;
	f_vida = vertex[0].vida;
	gl_TexCoord[0].x = t2.x;
	gl_TexCoord[0].y = t2.y;
	gl_FrontColor = gl_FrontColorIn[0];
	EmitVertex();
	
	gl_Position =  (p4);
	f_velocidad = vel2;
	f_vida = vertex[0].vida;
	gl_TexCoord[0].x = t4.x;
	gl_TexCoord[0].y = t4.y;
	gl_FrontColor = gl_FrontColorIn[0];
	EmitVertex();

	gl_Position =  (p3);
	f_velocidad = vel2;
	f_vida = vertex[0].vida;
	gl_TexCoord[0].x = t3.x;
	gl_TexCoord[0].y = t3.y;
	gl_FrontColor = gl_FrontColorIn[0];
	EmitVertex();

}
