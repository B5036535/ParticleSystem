#version 430 core

uniform float near;
uniform float far;
in Vertex
{
	vec4 colour;
	vec3 normal;
} IN;

out vec4 fragColour[3];

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main (void)
{
	fragColour[0] = IN.colour;
	fragColour[1] = vec4(IN.normal * 0.5 + 0.5, 1.0f);
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
    fragColour[2] = vec4(vec3(depth), 1.0);

}