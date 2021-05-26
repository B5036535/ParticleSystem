#version 430 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform bool SSBOswitch;

in vec3 position;


struct Particle
{
	float	lifeTime;
	vec4	colour;
	vec3	position;
	vec3	velocity;
	vec3	force;
};

layout (std430, binding  = 1) buffer SSBOStructA
{
	Particle particles[];
} particlesA;

layout (std430, binding  = 2) buffer SSBOStructB
{
	Particle particles[];
} particlesB;

out Vertex
{
	vec4 colour;
} OUT;

void main(void)
{

	vec3 pos;
	
	if(SSBOswitch)
	{
		pos = position + particlesB.particles[gl_InstanceID].position;
		OUT.colour = particlesB.particles[gl_InstanceID].colour;
	}
	else
	{
		pos = position + particlesA.particles[gl_InstanceID].position;
		OUT.colour = particlesB.particles[gl_InstanceID].colour;
	}
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(pos, 1.0);

}