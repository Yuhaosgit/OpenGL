#version 330 core

uniform samplerCube environmentMap;

in Vertex{
	vec3 viewDir;
	vec2 texCoord;
} IN;

out vec4 fragColour;

#define PI 3.1415926

void main(void) {
	vec3 irradiance = vec3(0.0);

	vec3 normal = IN.viewDir;
	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));
	
	float delta = 0.025;
	float sampleNum = 0.0; 
	for(float phi = 0.0; phi < 2.0 * PI; phi += delta)
	{
		for(float theta = 0.0; theta < 0.5 * PI; theta += delta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

			irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
			sampleNum++;
		}
	}
	irradiance = PI * irradiance * (1.0 / float(sampleNum));
	fragColour = vec4(irradiance, 1.0);
}