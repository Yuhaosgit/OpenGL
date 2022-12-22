#version 330 core

uniform sampler2D diffuseTex;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} IN;

out vec4 colorOutput;

void main() {
	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.bitangent), normalize(IN.normal));

	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	colorOutput = diffuse;
}