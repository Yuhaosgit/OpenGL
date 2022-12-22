#version 330 core

uniform sampler2D normTex;
uniform sampler2D depthTex;

uniform vec2 pixelSize; // reciprocal of resolution
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform mat4 inverseProjView;

uniform bool stencilPass;

layout(location = 0) out vec4 diffuseOutput;
layout(location = 1) out vec4 specularOutput;

void main(void) {
	if (stencilPass) {
		diffuseOutput = vec4(0, 0, 0, 0);
		specularOutput = vec4(0, 0, 0, 0);
		return;
	}
		
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth = texelFetch(depthTex, ivec2(gl_FragCoord.xy), 0).r;

	vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClipPos.xyz / invClipPos.w;

	float dist = length(lightPos - worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0)*0.15;

	if (atten == 0.0) {
		discard;
	}

	vec3 normal = normalize(texelFetch(normTex, ivec2(gl_FragCoord.xy),0).xyz * 2.0 - 1.0);
	vec3 incident = normalize(lightPos - worldPos);
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	vec3 lambert = clamp(dot(incident, normal), 0.0, 1.0)* lightColour.xyz;
	float specFactor = clamp(dot(halfDir, normal), 0.0, 1.0);

	vec3 specular = pow(specFactor, 50.0) * lightColour.xyz;

	diffuseOutput = vec4(lambert* atten, 1.0);
	specularOutput = vec4(specular* atten, 1.0);
}