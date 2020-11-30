#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec2 offset;


in vec3 position;
in vec3 normal;
in vec2 texCoord;
out Vertex
{
	vec3 normal;
	vec2 texCoord;
} OUT;

void main()
{
	mat3 normalMatrix = transpose(mat3(modelMatrix));
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	OUT.normal = normalize(normalMatrix * normalize(normal));
	OUT.texCoord = texCoord + offset;

}