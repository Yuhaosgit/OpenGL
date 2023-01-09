#version 330 core

uniform sampler2D directLightTex;

uniform samplerCube cubeTex;
uniform sampler2D depthTex;

in Vertex{
	vec3 viewDir;
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void) {
	float depthVal = texelFetch(depthTex, ivec2(gl_FragCoord.xy), 0).r;
	if (depthVal == 1.000) {
		fragColour = texture(cubeTex, normalize(IN.viewDir));
	}

	else {
		vec3 directLightResult = texelFetch(directLightTex, ivec2(gl_FragCoord.xy), 0).xyz;
		fragColour = vec4(directLightResult, 1.0);
	}
	fragColour.rgb = pow(fragColour.rgb, vec3(1.0 / 2.2));
}