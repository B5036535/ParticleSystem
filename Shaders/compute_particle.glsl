#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform bool SSBOswitch;
uniform float dt;
uniform float time;
uniform int	EmissionType;
uniform vec3 EmissionData;


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



float Random(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}



void EmitterRing()
{
	//Go in a random direction on the xz plane with a distance of EmissionData.x
	//float random = Random(vec2(time, gl_GlobalInvocationID.x))  * 2 - 1;
	const float twoPi	= 3.14159 * 2;

	float random = particlesB.particles[gl_GlobalInvocationID.x].random.x * twoPi;
	vec2 direction = vec2(sin(random) ,cos(random));

	if(SSBOswitch)
		particlesB.particles[gl_GlobalInvocationID.x].position.xz = direction * EmissionData.x;
	else															
		particlesA.particles[gl_GlobalInvocationID.x].position.xz = direction * EmissionData.x;
}

void EmitterSphere()
{
	//Get a random position within a whose radius = EmissionData.x
	const float twoPi	= 3.14159 * 2;
	//float randomTheta		= Random(vec2(time, gl_GlobalInvocationID.x)) * pi;
	//float randomPhi		= Random(vec2(time, gl_GlobalInvocationID.x)) * halfPi;
	//float randomRadius	= Random(vec2(time, gl_GlobalInvocationID.x)) * EmissionData.x;

	float randomTheta	= particlesB.particles[gl_GlobalInvocationID.x].random.x * twoPi;
	float randomPhi		= particlesB.particles[gl_GlobalInvocationID.x].random.y * twoPi;
	float randomRadius	= particlesB.particles[gl_GlobalInvocationID.x].random.z * EmissionData.x;


	float randomX = sin(randomTheta) * cos(randomPhi) * randomRadius;
	float randomY = sin(randomTheta) * sin(randomPhi) * randomRadius;
	float randomZ = cos(randomTheta) * randomRadius;

	if(SSBOswitch)
		particlesB.particles[gl_GlobalInvocationID.x].position.xyz = vec3(randomX, randomY, randomZ);
	else
		particlesA.particles[gl_GlobalInvocationID.x].position.xyz = vec3(randomX, randomY, randomZ);
}

void EmitterHemisphere()
{

	const float pi		= 3.14159;
	const float twoPi	= pi * 2;
	//float randomTheta		= Random(vec2(time, gl_GlobalInvocationID.x)) * pi;
	//float randomPhi		= Random(vec2(time, gl_GlobalInvocationID.x)) * halfPi;
	//float randomRadius	= Random(vec2(time, gl_GlobalInvocationID.x)) * EmissionData.x;

	float randomTheta	= particlesB.particles[gl_GlobalInvocationID.x].random.x * pi;
	float randomPhi		= particlesB.particles[gl_GlobalInvocationID.x].random.y * pi;
	float randomRadius	= particlesB.particles[gl_GlobalInvocationID.x].random.z * EmissionData.x;

	float randomX = sin(randomTheta) * cos(randomPhi) * randomRadius;
	float randomY = sin(randomTheta) * sin(randomPhi) * randomRadius;
	float randomZ = cos(randomTheta) * randomRadius;	

	if(SSBOswitch)
		particlesB.particles[gl_GlobalInvocationID.x].position.xyz = vec3(randomX, randomY, randomZ);
	else
		particlesA.particles[gl_GlobalInvocationID.x].position.xyz = vec3(randomX, randomY, randomZ);
}

void EmitterCube()
{
	//Get a random position within the extremeties of EmissionData

	//float randomX = Random(vec2(time, gl_GlobalInvocationID.x)) * 2 * EmissionData.x - EmissionData.x; 
	//float randomY = Random(vec2(time, randomX)) * 2 * EmissionData.y - EmissionData.y;
	//float randomZ = Random(vec2(time, randomY)) * 2 * EmissionData.z - EmissionData.z;

	float randomX = (particlesB.particles[gl_GlobalInvocationID.x].random.x * 2 * EmissionData.x) - EmissionData.x; 
	float randomY = (particlesB.particles[gl_GlobalInvocationID.x].random.y * 2 * EmissionData.y) - EmissionData.y;
	float randomZ = (particlesB.particles[gl_GlobalInvocationID.x].random.z * 2 * EmissionData.z) - EmissionData.z;

	if(SSBOswitch)
		particlesB.particles[gl_GlobalInvocationID.x].position.xyz = vec3(randomX, randomY, randomZ);
	else																						  
		particlesA.particles[gl_GlobalInvocationID.x].position.xyz = vec3(randomX, randomY, randomZ);
	
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
	particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz = particlesA.particles[gl_GlobalInvocationID.x].initvelocity.xyz;
	particlesA.particles[gl_GlobalInvocationID.x].force.xyz = particlesA.particles[gl_GlobalInvocationID.x].initforce.xyz;	
	particlesA.particles[gl_GlobalInvocationID.x].velocity.w = 0.0;
	particlesA.particles[gl_GlobalInvocationID.x].force.w = 0.0;
	particlesA.particles[gl_GlobalInvocationID.x].position.w = 1.0;

	particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz = particlesB.particles[gl_GlobalInvocationID.x].initvelocity.xyz;
	particlesB.particles[gl_GlobalInvocationID.x].force.xyz = particlesB.particles[gl_GlobalInvocationID.x].initforce.xyz;
	particlesB.particles[gl_GlobalInvocationID.x].velocity.w = 0.0;
	particlesB.particles[gl_GlobalInvocationID.x].force.w = 0.0;
	particlesB.particles[gl_GlobalInvocationID.x].position.w = 1.0;
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
	float red	= 1;
	float green = 1;
	float blue	= 1;
	float alpha;

	if(SSBOswitch)
	{
		//if(particlesA.particles[gl_GlobalInvocationID.x].lifetime.y < particlesA.particles[gl_GlobalInvocationID.x].lifetime.x)
		//	alpha = 1;
		//else
		//	alpha = 0;

		alpha = particlesA.particles[gl_GlobalInvocationID.x].position.w;
		particlesB.particles[gl_GlobalInvocationID.x].colour = vec4(red,green,blue,alpha);
	}

	else
	{
		//if(particlesB.particles[gl_GlobalInvocationID.x].lifetime.y < particlesB.particles[gl_GlobalInvocationID.x].lifetime.x)
		//	alpha = 1;
		//else
		//	alpha = 0;
		alpha = particlesB.particles[gl_GlobalInvocationID.x].position.w;

		particlesA.particles[gl_GlobalInvocationID.x].colour = vec4(red,green,blue,alpha);
	}
}

void UpdateLifeTime()
{
	if(SSBOswitch)
	{		
		if(particlesA.particles[gl_GlobalInvocationID.x].position.w == 1.0)
		{
			IntegrateAcceleration();
			IntegreatVelocity();

			if(particlesA.particles[gl_GlobalInvocationID.x].velocity.w < particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w)
				particlesB.particles[gl_GlobalInvocationID.x].velocity.w = particlesA.particles[gl_GlobalInvocationID.x].velocity.w + dt;

			else
			{
				particlesA.particles[gl_GlobalInvocationID.x].position.w = 0.0;
				particlesB.particles[gl_GlobalInvocationID.x].position.w = 0.0;
			}

		}
		else
		{
			if(particlesA.particles[gl_GlobalInvocationID.x].force.w < particlesA.particles[gl_GlobalInvocationID.x].initforce.w)
				particlesB.particles[gl_GlobalInvocationID.x].force.w = particlesA.particles[gl_GlobalInvocationID.x].force.w + dt;
			
			else
			{
				ResetParticle();
				Emission();
			}
		}
	}												  												  
	else											  
	{
		if(particlesB.particles[gl_GlobalInvocationID.x].position.w == 1.0)
		{
			IntegrateAcceleration();
			IntegreatVelocity();

			if(particlesB.particles[gl_GlobalInvocationID.x].velocity.w < particlesB.particles[gl_GlobalInvocationID.x].initvelocity.w)
				particlesA.particles[gl_GlobalInvocationID.x].velocity.w = particlesB.particles[gl_GlobalInvocationID.x].velocity.w + dt;

			else
			{
				particlesA.particles[gl_GlobalInvocationID.x].position.w = 0.0;
				particlesB.particles[gl_GlobalInvocationID.x].position.w = 0.0;
			}
		}
		else
		{
			if(particlesB.particles[gl_GlobalInvocationID.x].force.w < particlesB.particles[gl_GlobalInvocationID.x].initforce.w)
				particlesA.particles[gl_GlobalInvocationID.x].force.w = particlesB.particles[gl_GlobalInvocationID.x].force.w + dt;
							
			else
			{
				ResetParticle();
				Emission();
			}
		}
	}
}
void main(void)
{
		
	UpdateLifeTime();	
	CalculateColour();	
}