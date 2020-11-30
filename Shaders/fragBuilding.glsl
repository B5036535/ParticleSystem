#version 330 core

uniform sampler2D sideTex;
uniform sampler2D topTex;



in Vertex
{
	vec3 normal;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
	if(IN.normal.y != 0)
	{
		fragColour = texture(topTex, IN.texCoord);
	}
	else
	{
		fragColour = texture(sideTex, IN.texCoord);
	}
}