#version 430 core

uniform sampler2D displayTex;
uniform int appearanceType;

in Vertex
{
	vec4 colour;
	vec2 texCoord;
	float enabled;
} IN;

out vec4 fragColour;

void main (void)
{
	if(appearanceType == 1 && IN.enabled == 1.f)
		fragColour = texture2D(displayTex, IN.texCoord);
	else
		fragColour = IN.colour;
}