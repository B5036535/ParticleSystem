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
	if(IN.normal.y != 0)
	{
		fragColour[0] = texture(topTex, IN.texCoord + offset);
	}
	else
	{
		fragColour[0] = texture(sideTex, IN.texCoord + offset);
	}

	fragColour[1] = vec4(IN.normal * 0.5 + 0.5, 1.0f);

}