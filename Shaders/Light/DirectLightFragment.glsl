#version 330 core

uniform sampler2D normTex;
uniform sampler2D depthTex;
uniform sampler2D shadowTex;

uniform vec2 pixelSize; // reciprocal of resolution
uniform vec3 cameraPos;

uniform vec3 lightOrientation;
uniform vec4 lightColour;

uniform mat4 inverseProjView;
uniform mat4 shadowMatrix;

layout(location = 0) out vec4 diffuseOutput;
layout(location = 1) out vec4 specularOutput;

vec3 poissonDisk[4] = vec3[](
	vec3(-0.94201624, -0.39906216, 0.0000000),
	vec3(0.94558609, -0.76890725, 0.0000000),
	vec3(-0.094184101, -0.92938870, 0.0000000),
	vec3(0.34495938, 0.29387760, 0.0000000)
	);

void main(void) {
	//trans to world coordinate
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	vec4 stencilDepth = texelFetch(depthTex, ivec2(gl_FragCoord.xy), 0);
	if (stencilDepth.a != 1)
		discard;
	float depth = stencilDepth.r;

	vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClipPos.xyz / invClipPos.w;

	//shadow
	vec4 lightSpace = shadowMatrix * vec4(worldPos, 1.0);
	vec3 lightCoord = (lightSpace.xyz / lightSpace.w) * 0.5 + 0.5;
	float currentDepth = lightCoord.z;
	float closetDepth = texture(shadowTex, lightCoord.xy).r;

	float visibility = (closetDepth < currentDepth ? 0 : 1);
	if (currentDepth > 1.0)
		visibility = 1;

	//for (int i = 0; i < 4; i++) {
	//	if (texture(shadowTex, lightCoord + poissonDisk[i] / 2300.0) < 1) {
	//		visibility -= 0.25;
	//	}
	//}
	
	//specular and diffuse
	vec3 normal = normalize(texelFetch(normTex, ivec2(gl_FragCoord.xy), 0).xyz * 2.0 - 1.0);

	vec3 incident = lightOrientation;
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float lambert = clamp(dot(incident, normal), 0.0, 1.0);
	float specFactor = clamp(dot(halfDir, normal), 0.0, 1.0);

	specFactor = pow(specFactor, 60.0);

	diffuseOutput = vec4(lightColour.xyz * lambert * visibility, 1.0);
	specularOutput = vec4(lightColour.xyz * specFactor * 0.33 * visibility, 1.0);
}