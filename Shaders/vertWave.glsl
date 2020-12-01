#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform float time;


in vec3 position;
in vec2 texCoord;


out Vertex
{
	vec3 pos;
} OUT;

void main(void)
{
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	vec3 pos = position;

	pos.y = clamp(pos.y * sin(time *1.5f), 0.0f, 200.0f);
	

	gl_Position = mvp * vec4(pos, 1.0);
	OUT.pos = pos;
}

