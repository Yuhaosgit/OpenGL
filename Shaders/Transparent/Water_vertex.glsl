#version 330 core

uniform mat4 modelMatrix;
uniform mat4 textureMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

uniform float _Time;
uniform float _Speed;
uniform float _Length;
uniform vec2 _Center;
uniform float _Amplitude;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

#define PI 3.141592

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} OUT;

float SinWave(float amplitude, float length, vec2 direction, float speed, float time, vec2 position) {
	float w = (2 * PI) /length;
	return amplitude * sin(w * dot(direction, position) + time * speed * w);
}

float NormalFactor(float amplitude, float length, vec2 direction, float speed, float time, vec2 position) {
	float w = (2 * PI) / length;
	return w * amplitude * cos(w * dot(direction, position) + time * speed * w);
}

void main(){
	OUT.colour = colour;

	vec3 Normal = normalize(mat3(modelMatrix) * normalize(normal));
	vec3 Tangent  =  normalize(mat3(modelMatrix) * normalize(tangent.xyz));
	vec3 Bitangent = normalize(cross(Normal, Tangent)) * tangent.w;

	vec2 direction = (_Center - position.xz)/length(_Center - position.xz);
	float normalFac = NormalFactor(_Amplitude, _Length, direction, _Speed, _Time, position.xz);

	vec3 wTangent = vec3(1, direction.y * normalFac, 0);
	vec3 wBitangent = vec3(0, direction.x * normalFac, 1);
	vec3 wNormal = normalize(cross(wBitangent, wTangent));

	OUT.normal = wNormal + Normal;
	OUT.tangent = wTangent + Tangent;
	OUT.binormal = wBitangent + Bitangent;

	OUT.texCoord = (textureMatrix * vec4(texCoord, 0, 1)).xy;

	vec4 world_pos = modelMatrix * vec4(position + SinWave(_Amplitude, _Length, direction, _Speed, _Time, position.xz),1);
	OUT.worldPos = world_pos.xyz;

	gl_Position = projMatrix * viewMatrix * world_pos;
}