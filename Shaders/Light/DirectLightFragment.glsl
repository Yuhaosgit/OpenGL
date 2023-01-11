#version 400 core

uniform sampler2D ColorRoughnessTex;
uniform sampler2D normalMetallicTex;
uniform sampler2D depthTex;
uniform sampler2D shadowTex;
uniform sampler2D LUT;  

uniform samplerCube prefilterMap;
uniform samplerCube enviDiffuseTex;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform vec3 lightOrientation;
uniform vec4 lightColour;
uniform float lightIntensity;

uniform mat4 inverseProjView;
uniform mat4 shadowMatrix;
uniform float lightWidth;
uniform float nearPlane;

uniform bool HDR;
uniform bool enviDiffuseExist = true;

layout(location = 0) out vec4 FinalColor;

#define shadowPixelSize 1.0 / 2048.0
#define PI 3.1415926
#define saturateMediump(x) min(x, 65025.0)

float pow5(float value){
	float result = value * value;
	result *= result;
	result *= value;
	return result;
}

//*************************************shadow************************************************//
float PCF(vec3 lightCoord, int sampleNum) {
	float total = 0.0;

	for (int i = -sampleNum; i <= sampleNum; ++i) {
		for (int j = -sampleNum; j <= sampleNum; ++j) {
			vec2 offset = vec2(i, j) * shadowPixelSize;
			if (texture(shadowTex, lightCoord.xy + offset).r < lightCoord.z) {
				total += 1.0;
			}
		}
	}
	
	float iterTime = (float(sampleNum) * 2.0 + 1.0) * (float(sampleNum) * 2.0 + 1.0);
	return 1.0 - (total / iterTime);
}

void AverageBlockerDepth(vec3 lightCoord, float lightWidth, out float averageBlockDist, out float total)
{
	int filterTime = 3;
	float sumBlcokDist = 0.0;
	float receiverDist = lightCoord.z;
	float searchRegion = lightWidth * (receiverDist - nearPlane) / receiverDist;

	for (int i = -filterTime; i <= filterTime; ++i) {
		for (int j = -filterTime; j <= filterTime; ++j) {
			float bound = searchRegion / float(filterTime);
			vec2 offset = vec2(i * bound, j * bound) * shadowPixelSize;
			float shadowDepth = texture(shadowTex, lightCoord.xy + offset).r;

			if (shadowDepth < receiverDist) {
				sumBlcokDist += shadowDepth;
				total += 1.0;
			}
		}
	}
	averageBlockDist = sumBlcokDist / (total + 0.0001);
}

float PCSS(vec3 lightCoord, float lightWidth) {
	float receiverDist = lightCoord.z;
	float averageBlockDist;
	float total;
	AverageBlockerDepth(lightCoord, lightWidth, averageBlockDist, total);

	if (total < 1.0)
		return 1.0;

	float penumbraWidth = (receiverDist - averageBlockDist) * lightWidth / averageBlockDist;

	int range = clamp(int(penumbraWidth / 0.09), 2, 20);
	return PCF(lightCoord, range);
}
//*******************************************************************************************//

//*********************************Direction Light******************************************//
float DiffuseBurley(float roughness, float NoV, float NoL, float VoH) {
	float FD90 = 0.5 + 2.0 * VoH * VoH * roughness;
	float FdV = 1.0 + (FD90 - 1.0) * pow5(1.0 - NoV);
	float FdL = 1.0 + (FD90 - 1.0) * pow5(1.0 - NoL);

	return (1.0 / PI) * FdV * FdL;
}

float D_GGX(float roughness, float NoH)
{    
	float oneMinusNoHSquared = 1.0 - NoH * NoH;

    float a = NoH * roughness;
    float k = roughness / (oneMinusNoHSquared + a * a);
    float d = k * k * (1.0 / PI);

    return saturateMediump(d);
}

vec3 F_Schlick(float VoH, vec3 F0)
{
    return F0 + (vec3(1.0) - F0) * pow5(1.0 - VoH);
}

float smithG_GGX(float NoV, float NoL, float roughness)
{
    float a2 = roughness * roughness;

    float lambdaV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
    float lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
	float v = 0.5 / (lambdaV + lambdaL + 0.00001);

    return v;
}

vec3 DirectionalLightColor(float NoV, float NoL, float VoH, float NoH, float roughness, float metallic, vec3 baseColor, vec3 F0){
	//diffuse
	float diffuseTerm =  DiffuseBurley(roughness, NoV, NoL, VoH);
	vec3 diffuseColor = baseColor * (1 - metallic);

	//specular
	float D = D_GGX(roughness, NoH);
	vec3 F = F_Schlick(VoH, F0);
	float G = smithG_GGX(NoV, NoL, roughness);

	//mix diffuse and specular	
	return (diffuseTerm * diffuseColor * (vec3(1.0) - F) + (D * G * F)) * lightColour.xyz * NoL * lightIntensity;
}


//**********************************indirect lighting*****************************************//
vec3 F_Schlick_Roughtness(float NoV, vec3 F0, float roughness) {
    vec3 t = max(vec3(1.0 - roughness), F0);
    return F0 + (t - F0) * pow5(clamp(1.0 - NoV, 0.0, 1.0));
}

vec3 IndirectionalLightColor(vec3 normal, vec3 viewDir, float NoV, float roughness, float metallic, vec3 baseColor, vec3 F0){
	if (!enviDiffuseExist)
		return vec3(0.0, 0.0, 0.0);
	vec3 irradiance = texture(enviDiffuseTex, reflect(-viewDir, normal)).rgb;
	vec3 KS = F_Schlick_Roughtness(NoV, F0, roughness);

	return irradiance * baseColor * (vec3(1.0) - KS) * (1.0 - metallic);
}
//*******************************************************************************************//

void main(void) {
	//construct world coordinate
	vec2 screenPos = vec2(gl_FragCoord.xy * pixelSize);
	float depth = texelFetch(depthTex, ivec2(gl_FragCoord.xy), 0).r;

	vec3 ndcPos = vec3(screenPos, depth) * 2.0 - 1.0;
	vec4 invClip = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClip.xyz / invClip.w;

	//shadow
	vec4 lightSpace = shadowMatrix * vec4(worldPos, 1.0);
	vec3 lightCoord = (lightSpace.xyz / lightSpace.w) * 0.5 + 0.5;

	float visibility = clamp(PCSS(lightCoord, lightWidth), 0.2, 1.0);
	if (lightCoord.z > 1.0)
		visibility = 1.0;
	
	//PBR directional Light
	vec4 normalMetallic = texelFetch(normalMetallicTex, ivec2(gl_FragCoord.xy), 0);
	vec4 colorRoughness = texelFetch(ColorRoughnessTex, ivec2(gl_FragCoord.xy), 0);

	vec3 normal = normalMetallic.xyz * 2.0 - 1.0;
	vec3 baseColor = colorRoughness.xyz;
	float roughness = colorRoughness.w;
	float metallic = normalMetallic.w;

	vec3 viewDir = normalize(cameraPos - worldPos);
	vec3 halfDir = normalize(lightOrientation + viewDir);
	float NoV = abs(dot(normal, viewDir)) + 0.00001;
	float NoL = clamp(dot(normal, lightOrientation), 0.0, 1.0);
	float NoH = clamp(dot(normal, halfDir), 0.0, 1.0);
	float VoH = clamp(dot(viewDir, halfDir), 0.0, 1.0);

	//direction and indirection
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, baseColor, metallic);

	vec3 directionalLightColor = DirectionalLightColor(NoV, NoL, VoH, NoH, roughness, metallic, baseColor, F0);
	vec3 indirectionalLightColor = IndirectionalLightColor(normal, viewDir, NoV, roughness, metallic, baseColor, F0);

	vec3 combineColor = directionalLightColor + indirectionalLightColor;
	combineColor = combineColor / (combineColor + vec3(1.0));

	FinalColor = vec4(combineColor * visibility, 1.0);
}