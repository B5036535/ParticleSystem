#version 430 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
in vec3 position;
in vec4 colour;
in vec2 texCoord;

layout (std430, binding  = 6) buffer SSBOStruct
{
	vec2 offsets[];
} varData;

out Vertex
{
	vec4 colour;
	vec2 texCoord;
} OUT;

void main(void)
{

	vec3 pos;
	
	pos.x= position.x  + varData.offsets[gl_InstanceID].x;
	pos.y= position.y  + varData.offsets[gl_InstanceID].y;
	pos.z = 0;


	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(pos, 1.0);

	OUT.colour = colour;
	OUT.texCoord = texCoord;
}