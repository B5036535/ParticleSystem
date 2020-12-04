#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

in Vertex
{
	vec2 texCoord;
} IN;

out vec4 fragColour[2];




void main(void)
{
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	vec3 light = texture(diffuseLight, IN.texCoord).xyz;
	vec3 specular = texture(specularLight, IN.texCoord).xyz;

	fragColour[0].xyz = diffuse * 0.3; 
	fragColour[0].xyz += diffuse * light; 
	fragColour[0].xyz += specular; 
	fragColour[0].a = 1.0;

	if(fragColour[0].x > 0.3 || fragColour[0].y > 0.3 || fragColour[0].z > 0.3)
	{
		fragColour[1] = fragColour[0];
	}
	else
	{
		fragColour[1] = vec4(0,0,0,1);
	}
}