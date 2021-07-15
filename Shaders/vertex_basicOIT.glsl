#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 colour;

out Vertex
{
	vec4 colour;
	vec3 cameraSpace;
} OUT;

void main(void)
{
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
	OUT.colour = colour;
	OUT.cameraSpace = (modelMatrix * viewMatrix * vec4(position,1.0)).xyz;
}