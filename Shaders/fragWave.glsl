#version 330 core

in Vertex
{
	vec3 pos;
} IN;

out vec4 fragColour;

void main(void)
{
	vec4 HDRColour =  mix(vec4(0, 1.0f, 1.0f, 1.0f) , vec4(0.8, 0.4, 0, 1), IN.pos.y /2 + 0.5) * 3.0f;
	fragColour = HDRColour / (1 + HDRColour);
	fragColour.a = 0.5;
}