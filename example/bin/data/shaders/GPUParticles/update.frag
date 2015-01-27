#version 120
#extension GL_ARB_texture_rectangle : enable

// ping pong inputs
uniform sampler2DRect particles0; //pos
uniform sampler2DRect particles1; //vel
uniform sampler2DRect particles2; //color
uniform sampler2DRect particles3; //pos_ini
uniform sampler2DRect particles4; //TEXTURE_COORDS
uniform sampler2DRect particles5; //SIZE_LIFE_MAXDST

// Tamaño de la pantalla
//uniform sampler2DRect texStaticColor; //static color
//uniform sampler2DRect texVectorField; //vector field
uniform sampler2DRect texOpticalFlow; //Optical Flow

uniform float minVelocidad;
uniform float damping;
uniform float vectorFieldScalar;
uniform float opticalFlowScalar;
uniform vec3 mouse;
uniform float radiusSquared;
uniform float elapsed;
uniform vec3 gravity;
uniform float backToOrigin;

vec2 get2DOff(sampler2DRect tex ,vec2 coord) {
    vec4 col = texture2DRect(tex, coord);
    if (col.w >0.95)  col.z=col.z*-1;
    return vec2(-1*(col.y-col.x),col.z);
}

void main()
{
    vec3 pos = texture2DRect(particles0, gl_TexCoord[0].st).xyz;
    vec3 vel = texture2DRect(particles1, gl_TexCoord[0].st).xyz;
    vec4 color = texture2DRect(particles2, gl_TexCoord[0].st).xyzw;
    vec4 posini = texture2DRect(particles3, gl_TexCoord[0].st).xyzw;
    vec4 texCoords = texture2DRect(particles4, gl_TexCoord[0].st).xyzw;
    float partSize = texture2DRect(particles5, gl_TexCoord[0].st).x;
    float partVida = texture2DRect(particles5, gl_TexCoord[0].st).y;
    float partMaxDst = texture2DRect(particles5, gl_TexCoord[0].st).z;
    
	//vec3 vecField = texture2DRect(texVectorField, pos.xy).xyz;
	vec3 oFlow = vec3(get2DOff(texOpticalFlow, pos.xy), 0.0);
	
	if (opticalFlowScalar > 0.0)
		vel = vel + oFlow * opticalFlowScalar;

//	if (vectorFieldScalar > 0.0)
//		vel = vel + vecField * vectorFieldScalar;
	
    // mouse attraction
    vec3 direction = mouse - pos.xyz;
    float distSquared = dot(direction, direction);
    float magnitude = 500.0 * (1.0 - distSquared / radiusSquared);
    vec3 force = step(distSquared, radiusSquared) * magnitude * normalize(direction);

	//Vuelta al origen
	vec3 directionOrigin = posini.xyz - pos;
	if (length(directionOrigin) > 1.0)
	{
		vec3 forceOrigin = backToOrigin * normalize(directionOrigin);
//	if (backToOrigin > 0)
		force = force + forceOrigin;
	}

    // gravity
    force += gravity;
    //force += vec3(0.0, -0.5, 0.0);
    
    // accelerate
    vel += elapsed * force;
    
    // bounce off the sides
    //vel.x *= step(abs(pos.x), 512.0) * 2.0 - 1.0;
    //vel.y *= step(abs(pos.y), 384.0) * 2.0 - 1.0;
    //vel.x *= step(abs(pos.x), 1280.0 / 2.0) * 2.0 - 1.0;
    //vel.y *= step(abs(pos.y), 800.0 / 2.0) * 2.0 - 1.0;
    if (pos.x > 1280)
	{
		vel.x = vel.x * -1.0;
		pos.x = 1280;
	}
    if (pos.x < 0 )
	{
		vel.x = vel.x * -1.0;
		pos.x = 0;
	}
    if (pos.y > 800)
	{
		vel.y = vel.y * -1.0;
		pos.y = 800;
	}
    if (pos.y < 0)
	{
		vel.y = vel.y * -1.0;
		pos.y = 0;
	}
    
	// si se ha salido irremediablemente de los bordes, la pongo en la posición inicial por la cara
	if (pos.x < 0 || pos.x > 1280 || pos.y < 0 || pos.y > 800)
		pos = posini.xyz;
	
    // damping
    //vel *= 0.995;
    vel *= damping;

	if (length(vel) < minVelocidad && length(pos - posini.xyz) < 2.0)
	{
		pos = posini.xyz;
		vel = vec3(0,0,0);
	}
	else
    {
		// move
		pos += elapsed * vel;
	}

	if (partVida > 0) partVida--;
	
    gl_FragData[0] = vec4(pos, 1.0);
    gl_FragData[1] = vec4(vel, 0.0);
    gl_FragData[2] = color;
    gl_FragData[3] = posini;
	gl_FragData[4] = texCoords;
	gl_FragData[5] = vec4(partSize, partVida, partMaxDst, 0.0);

}