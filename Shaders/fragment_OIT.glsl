#version 430 core


in Vertex
{
	vec4 colour;
	vec3 cameraSpace;
} IN;

out vec4 fragColour[2];

void main (void)
{
	vec4 colour = IN.colour;
	float z = IN.cameraSpace.z;
	float weight =	
	max
	(
		min
		(
			1.0, 
			(
				max
				(
					max
					(
						colour.r
						, colour.g
					)
					, colour.b
				)
			
				* colour.a
			)
		)
	, colour.a) 
	* clamp(0.03 / (1e-5 + pow(z / 200, 4.0)), 1e-2, 3e3);

	//fragColour = IN.colour;

	fragColour[0] = vec4(colour.rgb * colour.a, colour.a) * weight;
	fragColour[1].a = colour.a;
}