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

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(void)
{
	mat4 mvp = projMatrix * viewMatrix * modelMatrix;
	vec3 pos = position;

	float randFloat = rand(texCoord) * 2 - 1;

	pos.y = sin(randFloat + time); 
	
	

	gl_Position = mvp * vec4(pos, 1.0);
	OUT.pos = pos;
}

