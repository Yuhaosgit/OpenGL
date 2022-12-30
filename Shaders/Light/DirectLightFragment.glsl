#version 400 core

uniform sampler2D normTex;
uniform sampler2D depthTex;
uniform sampler2D shadowTex;

uniform vec2 pixelSize; // reciprocal of resolution
uniform vec3 cameraPos;

uniform vec3 lightOrientation;
uniform vec4 lightColour;

uniform mat4 inverseProjView;
uniform mat4 shadowMatrix;
uniform mat4 lightViewMatrix;

layout(location = 0) out vec4 diffuseOutput;
layout(location = 1) out vec4 specularOutput;

vec2 poissonDisk[16] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760),
	vec2(-0.91588581, 0.45771432),
	vec2(-0.81544232, -0.87912464),
	vec2(-0.38277543, 0.27676845),
	vec2(0.97484398, 0.75648379),
	vec2(0.44323325, -0.97511554),
	vec2(0.53742981, -0.47373420),
	vec2(-0.26496911, -0.41893023),
	vec2(0.79197514, 0.19090188),
	vec2(-0.24188840, 0.99706507),
	vec2(-0.81409955, 0.91437590),
	vec2(0.19984126, 0.78641367),
	vec2(0.14383161, -0.14100790)
);

float PCF(vec2 uv, float currentDist, int sampleNum) {
	float total = 0.0;
	float shadowPixelSize = 1.0 / 2048.0;

	for (int i = -sampleNum; i <= sampleNum; ++i) {
		for (int j = -sampleNum; j <= sampleNum; ++j) {
			vec2 offset = vec2(i, j) * shadowPixelSize;
			if (texture(shadowTex, uv + offset).r < currentDist) {
				total += 1.0;
			}
		}
	}
	
	float iterTime = (float(sampleNum) * 2.0 + 1.0) * (float(sampleNum) * 2.0 + 1.0);
	return 1.0 - (total / iterTime);
}

void AverageBlockerDepth(vec2 uv, float receiverDist, float lightWidth, out float averageBlockDist, out float total)
{
	float filterTime = 3.0;
	float sumBlcokDist = 0.0;
	float sampleNum = 16;
	float shadowPixelSize = 1.0 / 2048.0;
	float searchRegion = lightWidth * (receiverDist - 0.1) / receiverDist;

	//for (int i = 0; i < sampleNum; ++i) {
	//	vec2 offset = poissonDisk[i] * shadowPixelSize * searchRegion;
	//	float shadowDepth = texture(shadowTex, uv + offset).r;

	//	if (shadowDepth < receiverDist) {
	//		sumBlcokDist += shadowDepth;
	//		total += 1.0;
	//	}
	//}
	for (int i = -3; i <= 3; ++i) {
		for (int j = -3; j <= 3; ++j) {
			vec2 offset = vec2(i * searchRegion / 3.0, j * searchRegion / 3.0);
			float shadowDepth = texture(shadowTex, uv + offset).r;

			if (shadowDepth < receiverDist) {
				sumBlcokDist += shadowDepth;
				total += 1.0;
			}
		}
	}
	averageBlockDist = sumBlcokDist / total;
}

float PCSS(vec2 uv, vec3 worldPos, float receiverDist, float lightWidth) {
	float averageBlockDist;
	float total;
	AverageBlockerDepth(uv, receiverDist, lightWidth, averageBlockDist, total);

	if (total < 1.0)
		return 1.0;

	float penumbraWidth = (receiverDist - averageBlockDist) * lightWidth / averageBlockDist;

	int range = int(penumbraWidth * 12.0);
	range = range > 40 ? 40 : range;
	range = range < 1 ? 2 : range;

	float result = PCF(uv, receiverDist, range);
	return result;
}

void main(void) {
	//trans to world coordinate
	vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
	float depth = texelFetch(depthTex, ivec2(gl_FragCoord.xy), 0).r;

	vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClipPos.xyz / invClipPos.w;

	//shadow

	vec4 lightSpace = shadowMatrix * vec4(worldPos, 1.0);
	vec3 lightCoord = (lightSpace.xyz / lightSpace.w) * 0.5 + 0.5;

	float visibility = PCSS(lightCoord.xy, worldPos, lightCoord.z, 4.0);
	if (lightCoord.z > 1.0)
		visibility = 1.0;
	
	//specular and diffuse
	vec3 normal = normalize(texelFetch(normTex, ivec2(gl_FragCoord.xy), 0).xyz * 2.0 - 1.0);

	vec3 incident = lightOrientation;
	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(incident + viewDir);

	float lambert = clamp(dot(incident, normal), 0.0, 1.0);
	float specFactor = clamp(dot(halfDir, normal), 0.0, 1.0);

	specFactor = pow(specFactor, 60.0);

	//diffuseOutput = vec4(visibility, visibility, visibility, 1.0);
	//specularOutput = vec4(0.0, 0.0, 0.0, 1.0);
	diffuseOutput = vec4(lightColour.xyz * lambert * visibility, 1.0);
	specularOutput = vec4(lightColour.xyz * specFactor * 0.33 * visibility, 1.0);
}