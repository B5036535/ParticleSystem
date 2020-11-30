#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D holoTex;


in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
	vec4 posterColour =  texture(diffuseTex, IN.texCoord);
	vec4 holoColour = texture(holoTex, IN.texCoord);
	fragColour = texture(diffuseTex, IN.texCoord);
	fragColour.a = 0.2f;
}