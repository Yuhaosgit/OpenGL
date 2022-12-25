#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

uniform samplerCube cubeTex;
uniform sampler2D depthTex;

in Vertex{
	vec3 viewDir;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
	float depthVal = texelFetch(depthTex, ivec2(gl_FragCoord.xy), 0).r;
	if (depthVal == 1) {
		fragColour = texture(cubeTex, normalize(IN.viewDir));
	}

	else {
		vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
		vec3 light = texture(diffuseLight, IN.texCoord).xyz;
		vec3 specular = texture(specularLight, IN.texCoord).xyz;

		fragColour.xyz = diffuse * 0.0; // ambient
		fragColour.xyz += diffuse * light; // lambert
		fragColour.xyz += specular; // Specular
		fragColour.a = 1.0;
	}
}