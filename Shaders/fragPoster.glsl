#version 330 core

uniform sampler2D diffuseTex;


in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
	vec4 HDRColour =  texture(diffuseTex, IN.texCoord) * 15.0f;
	fragColour = HDRColour / (1+HDRColour);
	fragColour.a = 0.5f;

}