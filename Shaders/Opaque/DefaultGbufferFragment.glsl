#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D normalTex;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} IN;

layout(location = 0) out vec4 colorOutput;
layout(location = 1) out vec4 normalOutput;

void main() {
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.bitangent), normalize(IN.normal));

	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	vec3 normals = texture(normalTex, IN.texCoord).rgb * 2.0 -1.0;
	normals = normalize(TBN * normalize(normals));

	colorOutput = diffuse;
	normalOutput = vec4(normals.xyz * 0.5 + 0.5, 1.0);
}