#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform bool SSBOswitch;
uniform float dt;
uniform float time;
uniform ivec3 types;
uniform vec3 EmissionData;

uniform vec3	vec_force;
uniform vec3	vec_force_max;

uniform vec3	vec_velocity_linear;
uniform vec3	vec_velocity_linear_max;

uniform vec4	spline_force_x[8];
uniform vec4	spline_force_y[8];
uniform vec4	spline_force_z[8];

uniform vec4	spline_velocity_linear_x[8];
uniform vec4	spline_velocity_linear_y[8];
uniform vec4	spline_velocity_linear_z[8];

uniform vec4	spline_velocity_orbital_x[8];
uniform vec4	spline_velocity_orbital_y[8];
uniform vec4	spline_velocity_orbital_z[8];
uniform vec4	spline_velocity_orbital_r[8];

uniform vec4	vec_colour;

uniform vec4	spline_colour_r[8];
uniform vec4	spline_colour_g[8];
uniform vec4	spline_colour_b[8];
uniform vec4	spline_colour_a[8];

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float near;
uniform float far;

uniform sampler2D tex_depth;
uniform sampler2D tex_normals;

struct Particle
{
	vec4 colour;
	vec4 position;
	vec4 initvelocity;
	vec4 velocity;
	vec4 initforce;
	vec4 force;
	vec4 random;
	vec4 collision;
};

layout (std430, binding  = 1) buffer SSBOStructA
{
	Particle particles[];
} particlesA;

layout (std430, binding  = 2) buffer SSBOStructB
{
	Particle particles[];
} particlesB;

const float twoPi	= 3.14159 * 2;
const float pi		= 3.14159;


float Random(float x)
{
    return fract(sin(dot(vec2(x, time) ,vec2(12.9898,78.233))) * 43758.5453);
}

// inbetween points 1 and 6 (starting index 0)
float GetPointOnSpline(float t, vec4 spline[8])
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


	return  y;
}

float GetSplineValue(float normalizedAge, vec4 spline[8])
{
	float tracker = 0;
	int i = 1;	// only points 1 - 6 have lengths
	do
	{
		tracker += spline[i].w;
		i++;
	} while (tracker < normalizedAge && i < 7);

	if (i < 7  && tracker <= 1.0f)
	{
		tracker -= spline[i - 1].w;
		float remainder = normalizedAge - tracker;
		float progressInSection = remainder / spline[i - 1].w;
		return GetPointOnSpline(i + progressInSection, spline);
	}
	else
		return 0.f;
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
	float random = particlesB.particles[gl_GlobalInvocationID.x].random.x * twoPi;
	vec2 direction = vec2(sin(random) ,cos(random));

	if(SSBOswitch)
		particlesB.particles[gl_GlobalInvocationID.x].position.xz = direction * EmissionData.x;
	else															
		particlesA.particles[gl_GlobalInvocationID.x].position.xz = direction * EmissionData.x;
}

void EmitterSphere()
{
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

void EmissionVelocity()
{
	if(SSBOswitch)
	{
		if(particlesA.particles[gl_GlobalInvocationID.x].initvelocity.y == 1.0f)
		{
			vec3 dir = normalize(particlesB.particles[gl_GlobalInvocationID.x].position.xyz);
			particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz = dir * particlesA.particles[gl_GlobalInvocationID.x].initvelocity.x;
		}
		else if(particlesA.particles[gl_GlobalInvocationID.x].initvelocity.z == 1.0f)
		{
		
			float x = particlesA.particles[gl_GlobalInvocationID.x].random.x * 2.0f - 1.0f;
			float y = particlesA.particles[gl_GlobalInvocationID.x].random.y * 2.0f - 1.0f;
			float z = particlesA.particles[gl_GlobalInvocationID.x].random.z * 2.0f - 1.0f;
		
			vec3 dir = vec3(x, y, z);
			particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz = dir * particlesA.particles[gl_GlobalInvocationID.x].initvelocity.x;
		}
		else
		{
			vec3 dir = vec3(1, 0, 0);
			particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz = dir * particlesA.particles[gl_GlobalInvocationID.x].initvelocity.x;
		}
	}
	else
	{
		if(particlesA.particles[gl_GlobalInvocationID.x].initvelocity.y == 1.0f)
		{
			vec3 dir = normalize(particlesA.particles[gl_GlobalInvocationID.x].position.xyz);
			particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz = dir * particlesB.particles[gl_GlobalInvocationID.x].initvelocity.x;
		}
		else if(particlesA.particles[gl_GlobalInvocationID.x].initvelocity.z == 1.0f)
		{
		
			float x =particlesB.particles[gl_GlobalInvocationID.x].random.x * 2.0f - 1.0f;
			float y =particlesB.particles[gl_GlobalInvocationID.x].random.y * 2.0f - 1.0f;
			float z =particlesB.particles[gl_GlobalInvocationID.x].random.z * 2.0f - 1.0f;
		
			vec3 dir = vec3(x, y, z);
			particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz = dir * particlesB.particles[gl_GlobalInvocationID.x].initvelocity.x;
		}
		else
		{
			vec3 dir = vec3(1, 0, 0);
			particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz = dir * particlesB.particles[gl_GlobalInvocationID.x].initvelocity.x;
		}
	}
}

void Emission()
{
	if(types.x == 1)
	{
		EmitterRing();
	}
	else if(types.x == 2)
	{
		EmitterCube();
	}
	else if(types.x == 4)
	{
		EmitterSphere();
	}
	else if(types.x == 8)
	{
		EmitterHemisphere();
	}
	else if(types.x == 16)
	{
		EmitterCone();
	}
}

void ResetParticle()
{
	particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz = vec3(0,0,0);
	particlesA.particles[gl_GlobalInvocationID.x].force.xyz = particlesA.particles[gl_GlobalInvocationID.x].initforce.xyz;	
	particlesA.particles[gl_GlobalInvocationID.x].velocity.w = 0.0;
	particlesA.particles[gl_GlobalInvocationID.x].force.w = 0.0;
	particlesA.particles[gl_GlobalInvocationID.x].position.w = 1.0;
	particlesA.particles[gl_GlobalInvocationID.x].collision.y = 0.0;

	particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz = vec3(0,0,0);
	particlesB.particles[gl_GlobalInvocationID.x].force.xyz = particlesB.particles[gl_GlobalInvocationID.x].initforce.xyz;
	particlesB.particles[gl_GlobalInvocationID.x].velocity.w = 0.0;
	particlesB.particles[gl_GlobalInvocationID.x].force.w = 0.0;
	particlesB.particles[gl_GlobalInvocationID.x].position.w = 1.0;
	particlesB.particles[gl_GlobalInvocationID.x].collision.y = 0.0;
}

vec3 GetSplineForce()
{
	if(SSBOswitch)
	{
		float normalizedAge = particlesA.particles[gl_GlobalInvocationID.x].velocity.w /particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w;
		return vec3(GetSplineValue(normalizedAge, spline_force_x), GetSplineValue(normalizedAge, spline_force_y), GetSplineValue(normalizedAge, spline_force_z));
	}
	else
	{
		float normalizedAge = particlesB.particles[gl_GlobalInvocationID.x].velocity.w /particlesB.particles[gl_GlobalInvocationID.x].initvelocity.w;
		return vec3(GetSplineValue(normalizedAge, spline_force_x), GetSplineValue(normalizedAge, spline_force_y), GetSplineValue(normalizedAge, spline_force_z));
	}
}



vec3 GetSplineVelocityLinear()
{
	if(SSBOswitch)
	{
		float normalizedAge = particlesA.particles[gl_GlobalInvocationID.x].velocity.w /particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w;
		return vec3(GetSplineValue(normalizedAge, spline_velocity_linear_x), GetSplineValue(normalizedAge, spline_velocity_linear_y), GetSplineValue(normalizedAge, spline_velocity_linear_z));
	}

	else
	{
		float normalizedAge = particlesB.particles[gl_GlobalInvocationID.x].velocity.w /particlesB.particles[gl_GlobalInvocationID.x].initvelocity.w;
		return vec3(GetSplineValue(normalizedAge, spline_velocity_linear_x), GetSplineValue(normalizedAge, spline_velocity_linear_y), GetSplineValue(normalizedAge, spline_velocity_linear_z));
	}
}

vec3 GetSplineVelocityOrbital()
{
	if(SSBOswitch)
	{
		float normalizedAge = particlesA.particles[gl_GlobalInvocationID.x].velocity.w /particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w;
		return vec3(GetSplineValue(normalizedAge, spline_velocity_orbital_x), GetSplineValue(normalizedAge, spline_velocity_orbital_y), GetSplineValue(normalizedAge, spline_velocity_orbital_z));
	}

	else
	{
		float normalizedAge = particlesB.particles[gl_GlobalInvocationID.x].velocity.w /particlesB.particles[gl_GlobalInvocationID.x].initvelocity.w;
		return vec3(GetSplineValue(normalizedAge, spline_velocity_orbital_x), GetSplineValue(normalizedAge, spline_velocity_orbital_y), GetSplineValue(normalizedAge, spline_velocity_orbital_z));
	}
}

vec4 GetSplineColour()
{
	if(SSBOswitch)
	{
		float normalizedAge = particlesA.particles[gl_GlobalInvocationID.x].velocity.w /particlesA.particles[gl_GlobalInvocationID.x].initvelocity.w;
		return vec4(GetSplineValue(normalizedAge, spline_colour_r), GetSplineValue(normalizedAge, spline_colour_g), GetSplineValue(normalizedAge, spline_colour_b), GetSplineValue(normalizedAge, spline_colour_a));
	}

	else
	{
		float normalizedAge = particlesB.particles[gl_GlobalInvocationID.x].velocity.w /particlesB.particles[gl_GlobalInvocationID.x].initvelocity.w;
		return vec4(GetSplineValue(normalizedAge, spline_colour_r), GetSplineValue(normalizedAge, spline_colour_g), GetSplineValue(normalizedAge, spline_colour_b), GetSplineValue(normalizedAge, spline_colour_a));
	}
}


void AddOrbitalVelocity()
{
	if(SSBOswitch)
	{
		vec3 dir = -particlesA.particles[gl_GlobalInvocationID.x].position.xyz;
	}
	else
	{
		vec3 dir = -particlesB.particles[gl_GlobalInvocationID.x].position.xyz;
		
	}
}

vec3 GetForce()
{
	if(types.y == 0)
		return vec_force;

	if(types.y == 1)
		return vec_force;

	else
		return GetSplineForce();

}

vec3 GetLinearVelocity()
{
	if(types.y == 0)
		return vec_velocity_linear;

	if(types.y == 1)
		return vec_velocity_linear;

	else
		return GetSplineVelocityLinear();
}
void IntegrateAcceleration()
{
	if(SSBOswitch)
	{
		vec3 dVel = GetSplineForce() * dt;
		vec3 vel = particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz + GetSplineVelocityLinear() + dVel;
		particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz = vel;
	}

	else
	{
		vec3 dVel = GetSplineForce() * dt;
		vec3 vel = particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz + GetSplineVelocityLinear() + dVel;
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

void OnCollision()
{
	particlesA.particles[gl_GlobalInvocationID.x].collision.y = 1.0f;
	particlesB.particles[gl_GlobalInvocationID.x].collision.y = 1.0f;

	particlesA.particles[gl_GlobalInvocationID.x].colour.rgb = vec3(1.0f,0.0f,0.0f);
	particlesB.particles[gl_GlobalInvocationID.x].colour.rgb = vec3(1.0f,0.0f,0.0f);
}

void ResolveCollision(vec2 texCoords, float penetration)
{
	vec3 normal = texture2D(tex_normals, texCoords).rgb * 2.0f - 1.0f;
	float cRestitution = 1.0f; //fully elastic
	if(SSBOswitch)
	{
		particlesB.particles[gl_GlobalInvocationID.x].position.xyz += penetration * normal;
		float impulseForce = dot(particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz, normal.rgb);
		float j = (-(1.0f + cRestitution) * impulseForce);
		vec3 fullImpulse = normal * j;
		particlesA.particles[gl_GlobalInvocationID.x].velocity.xyz += fullImpulse;
	}
	else
	{
		particlesA.particles[gl_GlobalInvocationID.x].position.xyz += penetration * normal;
		float impulseForce = dot(particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz, normal.rgb);
		float j = (-(1.0f + cRestitution) * impulseForce);
		vec3 fullImpulse = normal * j;
		particlesB.particles[gl_GlobalInvocationID.x].velocity.xyz += fullImpulse;
	}
}

float LinearDepth(float expDepth)
{
	float z  = expDepth * 2.0f - 1.0f;
	return (2.0f * near * far) + (far + near - z * (far - near));
}

void CollisionDetection()
{
	vec3 cameraSpace;
	if(SSBOswitch)
	{
		cameraSpace = (projMatrix * viewMatrix * modelMatrix * vec4(particlesA.particles[gl_GlobalInvocationID.x].position.xyz,1.0)).xyz;
	}
	else
	{
		cameraSpace = (projMatrix * viewMatrix * modelMatrix * vec4(particlesB.particles[gl_GlobalInvocationID.x].position.xyz,1.0)).xyz;
	}
	vec3 NDCSpace = cameraSpace.xyz * 2.0f - 1.0f;
	//float expDepth = texture2D(tex_depth, NDCSpace.xy).r;
	float depth = texture2D(tex_depth, NDCSpace.xy).r * far;
	
	if(NDCSpace.z > depth)
	{
		float penetration = (NDCSpace.z * -1.0f - depth) * (far - near);
		ResolveCollision(NDCSpace.xy, penetration);
		OnCollision();
	}
	
}

vec4 GetColour()
{
	if(types.z == 0)
		return vec_colour;

	if(types.z == 1)
		return vec_colour;

	else
		return GetSplineColour();

}

void CalculateColour()
{
	if(SSBOswitch)
	{
		particlesB.particles[gl_GlobalInvocationID.x].colour = GetColour();

		if(particlesA.particles[gl_GlobalInvocationID.x].position.w != 1.f)
			particlesB.particles[gl_GlobalInvocationID.x].colour.a = 0.f;
	}

	else
	{
		particlesA.particles[gl_GlobalInvocationID.x].colour = GetColour();

		if(particlesB.particles[gl_GlobalInvocationID.x].position.w != 1.f)
			particlesA.particles[gl_GlobalInvocationID.x].colour.a = 0.f;
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
				EmissionVelocity();
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
				EmissionVelocity();
			}
		}
	}
}
void main(void)
{
	if(particlesA.particles[gl_GlobalInvocationID.x].collision.x == 1.0f)
		CollisionDetection();

	UpdateLifeTime();

	if(particlesA.particles[gl_GlobalInvocationID.x].collision.y != 1.0f)
		CalculateColour();	
}