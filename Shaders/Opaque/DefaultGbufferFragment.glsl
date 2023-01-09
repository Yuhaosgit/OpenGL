#version 330 core

uniform sampler2D colorTex;
uniform sampler2D normalTex;
uniform sampler2D roughnessTex;
uniform sampler2D metallicTex;

uniform bool roughnessExist = true;
uniform bool metallicExist = true;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} IN;

layout(location = 0) out vec4 colorRoughnessOutput;
layout(location = 1) out vec4 normalMetallicOutput;

void main() {
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.bitangent), normalize(IN.normal));
	
	vec3 color = texture(colorTex, IN.texCoord).rgb;
	vec3 normals = TBN * (texture(normalTex, IN.texCoord).rgb * 2.0 - 1.0);

	float roughness = roughnessExist ? texture(roughnessTex, IN.texCoord).r : 0.5;
	float metallic = metallicExist ? texture(metallicTex, IN.texCoord).r : 0.0;

	colorRoughnessOutput = vec4(color, roughness);
	normalMetallicOutput = vec4(normals * 0.5 + 0.5, metallic);
}