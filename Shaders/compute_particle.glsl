#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform bool SSBOswitch;
uniform float dt;
uniform float time;
uniform int	EmissionType;
uniform vec3 EmissionData;

uniform vec4	spline_force[8];
uniform float	spline_force_totalLength;
uniform vec3	spline_force_maxValues;

uniform vec4	spline_colour[8];
uniform float	spline_colour_totalLength;
uniform vec3	spline_colour_maxValues;

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

// inbetween points 1 and 6 (starting index 0)
vec3 GetPointOnSpline(float t, vec4 spline[8])
{

	int p1 = int(floor(t));
	int p0 = int(p1 - 1);
	int p2 = int(p1 + 1);
	int p3 = int(p2 + 1);

	t = t - floor(t);
	
	float tSquared = t * t;
	float tCubed = tSquared * t;

	float influence[4];
	influence[0] = -tCubed + 2.0f * tSquared - t;
	influence[1] = 3.0f * tCubed - 5.0f * tSquared + 2.0f;
	influence[2] = -3.0f * tCubed + 4.0f * tSquared + t;
	influence[3] = tCubed - tSquared;

	
	
	float x = 0.5f * 
	(
		spline[p0].x * influence[0] 
		+ 
		spline[p1].x * influence[1] 
		+
		spline[p2].x * influence[2] 
		+
		spline[p3].x * influence[3]
	);


	float y = 0.5f * 
	(
		spline[p0].y * influence[0] 
		+ 
		spline[p1].y * influence[1] 
		+ 
		spline[p2].y * influence[2] 
		+ 
		spline[p3].y * influence[3]
	);

	float z = 0.5f * 
	(
		spline[p0].z * influence[0] 
		+ 
		spline[p1].z * influence[1] 
		+ 
		spline[p2].z * influence[2] 
		+ 
		spline[p3].z * influence[3]
	);
	return vec3(x, y, z);
}

float GetSplineTime(float splineTotalLength, vec4 spline[8])
{
	float lengthPerT = splineTotalLength / particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w;
	float currentLengthTravelled = particlesA.particles[gl_GlobalInvocationID.x].velocity.w * lengthPerT;

	float tracker = 0;
	int i = 1;	// only points 1 - 6 have lengths
	while(tracker < currentLengthTravelled && i < 7)
	{
		tracker += spline[i].w;
		i++;
	}

	if(i != 7)
	{
		tracker -= spline[i].w;
		float remainder = currentLengthTravelled - tracker;
		return float(i - 1) + remainder / spline[i].w;
	}
	else
		return 0.f;
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

void AddForce()
{
	//vec3 splineData = GetPointOnSpline(GetSplineTime(spline_force_totalLength, spline_force), spline_force);
	//
	//vec3 splineInfluence = vec3(splineData.x, splineData.y , splineData.z);


	if(SSBOswitch)
	{
		vec3 splineData = GetPointOnSpline(particlesA.particles[gl_GlobalInvocationID.x].velocity.w * 6 / particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w, spline_force);
		vec3 splineInfluence = vec3(splineData.x, splineData.y , splineData.z);

		particlesB.particles[gl_GlobalInvocationID.x].force.xyz = particlesA.particles[gl_GlobalInvocationID.x].force.xyz + normalize(particlesA.particles[gl_GlobalInvocationID.x].position.xyz) * splineInfluence * 0.2f;
	}

	else
	{
		vec3 splineData = GetPointOnSpline(particlesA.particles[gl_GlobalInvocationID.x].velocity.w * 6 / particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w, spline_force);
		vec3 splineInfluence = vec3(splineData.x, splineData.y , splineData.z);

		particlesA.particles[gl_GlobalInvocationID.x].force.xyz = particlesB.particles[gl_GlobalInvocationID.x].force.xyz + normalize(particlesB.particles[gl_GlobalInvocationID.x].position.xyz) * splineInfluence * 0.2f;
	}
}

void CalculateForce()
{
	float attractiveForce = 10f;
	if(SSBOswitch)
	{
		attractiveForce = length(particlesA.particles[gl_GlobalInvocationID.x].position.xyz) * 20.0f;
		particlesB.particles[gl_GlobalInvocationID.x].force.xyz = normalize(particlesA.particles[gl_GlobalInvocationID.x].position.xyz) * attractiveForce;
	}

	else
	{
		attractiveForce = length(particlesB.particles[gl_GlobalInvocationID.x].position.xyz) * 20.0f;
		particlesA.particles[gl_GlobalInvocationID.x].force.xyz =  normalize(particlesB.particles[gl_GlobalInvocationID.x].position.xyz) * attractiveForce;
	}
	
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

void AddVelocityFromSpline()
{
	//vec3 splineData = GetPointOnSpline(GetSplineTime(spline_force_totalLength, spline_force), spline_force);

	//vec3 splineInfluence = vec3(splineData.x / spline_force_maxValues.x, splineData.y / spline_force_maxValues.y, splineData.z / spline_force_maxValues.z );


	if(SSBOswitch)
	{
		vec3 splineData = GetPointOnSpline(particlesA.particles[gl_GlobalInvocationID.x].velocity.w * 6 / particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w, spline_force);
		vec3 splineInfluence = vec3(splineData.x, splineData.y , splineData.z  );
		
		particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz = particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz + normalize(particlesA.particles[gl_GlobalInvocationID.x].position.xyz) * splineInfluence * 1.5f;
	}

	else
	{
		vec3 splineData = GetPointOnSpline(particlesB.particles[gl_GlobalInvocationID.x].velocity.w * 6 / particlesB.particles[gl_GlobalInvocationID.x].initvelocity.w, spline_force);

		vec3 splineInfluence = vec3(splineData.x, splineData.y , splineData.z  );
		particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz = particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz + normalize(particlesB.particles[gl_GlobalInvocationID.x].position.xyz) * splineInfluence * 1.5f;
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

	vec3 splineData = GetPointOnSpline(GetSplineTime(spline_colour_totalLength, spline_colour), spline_colour);

	//splineData = clamp(splineData, vec3(0.0f,0.0f,0.0f), vec3(1.0f, 1.0f,1.0f));

	//float red = splineData.x;

	if(SSBOswitch)
	{
		//if(particlesA.particles[gl_GlobalInvocationID.x].lifetime.y < particlesA.particles[gl_GlobalInvocationID.x].lifetime.x)
		//	alpha = 1;
		//else
		//	alpha = 0;

		alpha = particlesA.particles[gl_GlobalInvocationID.x].position.w;
		particlesB.particles[gl_GlobalInvocationID.x].colour = vec4(red,green,blue,alpha);
		//particlesB.particles[gl_GlobalInvocationID.x].colour = vec4(splineData,alpha);
	}

	else
	{
		//if(particlesB.particles[gl_GlobalInvocationID.x].lifetime.y < particlesB.particles[gl_GlobalInvocationID.x].lifetime.x)
		//	alpha = 1;
		//else
		//	alpha = 0;
		alpha = particlesB.particles[gl_GlobalInvocationID.x].position.w;

		particlesA.particles[gl_GlobalInvocationID.x].colour = vec4(red,green,blue,alpha);
		//particlesA.particles[gl_GlobalInvocationID.x].colour = vec4(splineData,alpha);
	}
}

void UpdateLifeTime()
{
	if(SSBOswitch)
	{		
		if(particlesA.particles[gl_GlobalInvocationID.x].position.w == 1.0)
		{
			//AddForce();
			//CalculateForce();
			IntegrateAcceleration();
	
			AddVelocityFromSpline();
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
			//AddForce();
			//CalculateForce();
			IntegrateAcceleration();

			AddVelocityFromSpline();
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