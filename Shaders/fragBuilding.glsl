#version 330 core

uniform sampler2D sideTex;
uniform sampler2D topTex;
uniform vec2 offset;



in Vertex
{
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
} IN;

out vec4 fragColour[2];

void main(void)
{
vec4 HDRColour;
	if(IN.normal.y != 0)
	{
		HDRColour = texture(topTex, IN.texCoord + offset) * 2.0f;
		fragColour[0] = HDRColour / (1+ HDRColour);
	}
	else
	{
		HDRColour = texture(sideTex, IN.texCoord + offset) * 2.0f;

		fragColour[0] = HDRColour / (1+ HDRColour);
	}

	fragColour[1] = vec4(IN.normal * 0.5 + 0.5, 1.0f);

}