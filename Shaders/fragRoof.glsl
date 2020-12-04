#version 330 core



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
	fragColour[0] = vec4(0.0, 0.0, 0.0, 1.0);

	fragColour[1] = vec4(IN.normal * 0.5 + 0.5, 1.0f);
}