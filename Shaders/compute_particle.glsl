#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform bool SSBOswitch;
uniform float dt;
uniform float time;
uniform int	EmissionType;
uniform vec3 EmissionData;


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



void EmitterRing()
{
	//Go in a random direction on the xz plane with a distance of EmissionData.x
	float random = Random(vec2(time * 100, gl_GlobalInvocationID.x * 100))  * 2 - 1;
	vec2 direction = vec2(sin(random) ,cos(random));

	if(SSBOswitch)
		particlesB.particles[gl_GlobalInvocationID.x].position.xz = direction * EmissionData.x;
	else															
		particlesA.particles[gl_GlobalInvocationID.x].position.xz = direction * EmissionData.x;
}

void EmitterSphere()
{
	//Get a random position within a hemisphere whose radius = EmissionData.x

}

void EmitterHemisphere()
{
	
}

void EmitterCube()
{
	//Get a random position within the extremeties of EmissionData

	float randomX = Random(vec2(time, gl_GlobalInvocationID.x)) * 2 * EmissionData.x - EmissionData.x; 
	float randomY = Random(vec2(time, randomX)) * 2 * EmissionData.y - EmissionData.y;
	float randomZ = Random(vec2(time, randomY)) * 2 * EmissionData.z - EmissionData.z;

	if(SSBOswitch)
		particlesB.particles[gl_GlobalInvocationID.x].position.xyz = vec3(randomX, randomY, randomY);
	else
		particlesA.particles[gl_GlobalInvocationID.x].position.xyz = vec3(randomX, randomY, randomY);
	
}

void EmitterCone()
{
}

void Emission()
{
	if(EmissionType == 1)
	{
		EmitterRing();
	}
	else if(EmissionType == 2)
	{
		EmitterCube();
	}
	else if(EmissionType == 4)
	{
		EmitterSphere();
	}
	else if(EmissionType == 8)
	{
		EmitterHemisphere();
	}
	else if(EmissionType == 16)
	{
		EmitterCone();
	}
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
	float blue = sin(particlesA.particles[gl_GlobalInvocationID.x].velocity.y + 0.8);

	if(SSBOswitch)
	{

		particlesB.particles[gl_GlobalInvocationID.x].colour = vec4(red,green,blue,1);
	}

	else
	{
		particlesA.particles[gl_GlobalInvocationID.x].colour = vec4(red,green,blue,1);
	}
}

void main(void)
{

	
	if(SSBOswitch)
	{
		if(particlesA.particles[gl_GlobalInvocationID.x].force.w > -0.00833 && particlesA.particles[gl_GlobalInvocationID.x].force.w < 0.00833)
			Emission();
		
		//if(particlesA.particles[gl_GlobalInvocationID.x].force.w > 0)
		//{
		//	IntegrateAcceleration();
		//	IntegreatVelocity();
		//	CalculateColour();
		//}
		particlesB.particles[gl_GlobalInvocationID.x].force.w += dt;
	}												  
													  
	else											  
	{	
		if(particlesB.particles[gl_GlobalInvocationID.x].force.w > -0.00833 && particlesB.particles[gl_GlobalInvocationID.x].force.w < 0.00833)
			Emission();

		//if(particlesB.particles[gl_GlobalInvocationID.x].force.w > 0)
		//{
		//	IntegrateAcceleration();
		//	IntegreatVelocity();
		//	CalculateColour();
		//}
		particlesA.particles[gl_GlobalInvocationID.x].force.w += dt;
	}	
}