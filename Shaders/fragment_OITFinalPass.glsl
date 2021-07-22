#version 430 core

uniform sampler2D tex_accumulation;
uniform sampler2D tex_reveal;


in Vertex
{
	vec4 colour;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main (void)
{
	vec4 colour = IN.colour;
	
	vec4 accum = texture(tex_accumulation, IN.texCoord, 0);
	float r = texture(tex_reveal, IN.texCoord, 0).r;

	fragColour = vec4(accum.rgb / clamp(accum.a, 1e-4, 5e4), r);
}