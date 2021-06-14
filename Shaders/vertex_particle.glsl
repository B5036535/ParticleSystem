#version 430 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform bool SSBOswitch;

in vec3 position;


struct Particle
{
	vec4 colour;
	vec4 position;
	vec4 initvelocity;
	vec4 velocity;
	vec4 initforce;
	vec4 force;
	vec4 random;
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
		pos = position + particlesB.particles[gl_InstanceID].position.xyz;
		OUT.colour = particlesB.particles[gl_InstanceID].colour;
	}
	else
	{
		pos = position + particlesA.particles[gl_InstanceID].position.xyz;
		OUT.colour = particlesB.particles[gl_InstanceID].colour;
	}
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(pos, 1.0);

}