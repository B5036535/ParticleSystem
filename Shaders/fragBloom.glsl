#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D blurTex;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{

	vec4 HDRColour = vec4(texture(diffuseTex, IN.texCoord).xyz + texture(blurTex, IN.texCoord).xyz, 1.0);
	fragColour = HDRColour / (1 + HDRColour);
	fragColour.a = 1;

}