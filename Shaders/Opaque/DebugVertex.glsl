#version 400 core

uniform  mat4  modelMatrix;
uniform  mat4  viewMatrix;
uniform  mat4  projMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec3 tangent;
in vec3	bitangent;
in vec2 texCoord;

out  Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
} OUT;

void  main(void) {
	OUT.colour = colour;
	OUT.texCoord = texCoord;

	vec3 wNormal = normalize(mat3(modelMatrix) * normalize(normal));
	vec3 wTangent = normalize(mat3(modelMatrix) * normalize(tangent));
	vec3 wBitangent = normalize(mat3(modelMatrix) * normalize(bitangent));

	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.bitangent = wBitangent;

	vec4 worldPos = modelMatrix * vec4(position, 1.0f);
	gl_Position = projMatrix * viewMatrix * worldPos;
}