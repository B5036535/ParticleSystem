#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec3 normal;

out Vertex
{
	vec3 normal;
} OUT;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.normal = normalize(normalMatrix * normalize(normal));
}