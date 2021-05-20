#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout (std430, binding  = 6) buffer SSBOStruct
{
	vec2 offsets[];
} varData;


void main(void)
{
	varData.offsets[gl_GlobalInvocationID.x].x = varData.offsets[gl_GlobalInvocationID.x].x - 0.1;
}