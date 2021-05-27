#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform bool SSBOswitch;
uniform float dt;

struct Particle
{
	vec4	colour;
	vec4	position;
	vec4	velocity;
	vec4	force;
};

layout (std430, binding  = 1) buffer SSBOStructA
{
	Particle particles[];
} particlesA;

layout (std430, binding  = 2) buffer SSBOStructB
{
	Particle particles[];
} particlesB;

float Random(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void ResetParticle()
{
}

void IntegrateAcceleration()
{
	if(SSBOswitch)
	{
		vec3 dVel = particlesA.particles[gl_GlobalInvocationID.x].force.xyz * dt;
		vec3 vel = particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz + dVel;
		particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz = vel;
	}

	else
	{
		vec3 dVel = particlesB.particles[gl_GlobalInvocationID.x].force.xyz * dt;
		vec3 vel = particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz + dVel;
		particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz = vel;
	}
}

void IntegreatVelocity()
{
	if(SSBOswitch)
	{
		vec3 dPos = particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz * dt;
		vec3 pos = particlesA.particles[gl_GlobalInvocationID.x].position.xyz + dPos;
		particlesB.particles[gl_GlobalInvocationID.x].position.xyz = pos;
	}

	else
	{
		vec3 dPos = particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz * dt;
		vec3 pos = particlesB.particles[gl_GlobalInvocationID.x].position.xyz + dPos;
		particlesA.particles[gl_GlobalInvocationID.x].position.xyz = pos;
	}
}

void CalculateColour()
{
	float red = sin(particlesA.particles[gl_GlobalInvocationID.x].velocity.y);
	float green = cos(particlesA.particles[gl_GlobalInvocationID.x].velocity.y);
	float blue = sin(particlesA.particles[gl_GlobalInvocationID.x].velocity.y * 10);

	if(SSBOswitch)
	{

		particlesB.particles[gl_GlobalInvocationID.x].colour = vec4(red,green,blue,1);
	}

	else
	{
		particlesB.particles[gl_GlobalInvocationID.x].colour = vec4(red,green,blue,1);
	}
}

void main(void)
{
	IntegrateAcceleration();
	IntegreatVelocity();
	CalculateColour();
}