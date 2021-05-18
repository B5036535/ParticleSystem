#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
in vec3 position;
in vec4 colour;
in vec2 offset;

out Vertex
{
	vec4 colour;
} OUT;

void main(void)
{

	vec3 pos;
	
	pos.x= position.x * ((gl_InstanceID + 1) / 100) + offset.x;
	pos.y= position.y * ((gl_InstanceID + 1) / 100) + offset.y;
	pos.z = 0;

	//gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(pos, 1.0);
	gl_Position = vec4(offset, 0.0,1.0);
	OUT.colour = colour;
}