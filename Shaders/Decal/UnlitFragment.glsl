#version 330 core

layout(location = 0) out vec4 colorOutput;
layout(location = 1) out vec4 normalOutput;

uniform vec2 pixelSize;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

uniform sampler2D stencilDepthTex;
uniform sampler2D normals;

uniform sampler2D targetTex;
uniform sampler2D targetNormalTex;

void main(void) {
	vec2 ScreenCoord = vec2(gl_FragCoord.xy * pixelSize);
	vec4 stencilDepth = texelFetch(stencilDepthTex, ivec2(gl_FragCoord.xy), 0);
	if (stencilDepth.a == 0x00)
		discard;
	float depth = stencilDepth.r;

	mat4 inverseModel = inverse(modelMatrix);
	mat4 inverseProjView = inverse(projMatrix * viewMatrix);
	vec3 ndcPos = vec3(ScreenCoord, depth) * 2.0 - 1.0;
	vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
	vec3 worldPos = invClipPos.xyz / invClipPos.w;
	vec3 decalPos = (inverseModel * vec4(worldPos, 1)).xyz;
	if (abs(decalPos.x) > 0.5 || abs(decalPos.y) > 0.5 || abs(decalPos.z) > 0.5)
		discard;

	vec2 decalCoord = decalPos.xz;

	vec3 worldNormal = texture(normals, gl_FragCoord.xy).xyz;
	vec3 decalNormal = (inverseModel * vec4(worldNormal, 0)).xyz;

	decalCoord -= decalCoord.y * decalNormal.xz;
	decalCoord += 0.5;

	vec4 color = texture(targetTex, decalCoord);

	//normal process
	vec3 binormal = normalize(dFdx(decalPos));
	vec3 tangent = normalize(dFdy(decalPos));
	vec3 normal = normalize(cross(binormal, tangent));

	mat3 TBN = mat3(tangent, binormal, normal);
	vec3 bump = texture(targetNormalTex, decalCoord).xyz;

	bump = normalize(bump * 2 - 1);
	bump = normalize(TBN * bump);

	colorOutput = color;	
	//normalOutput = vec4(0,0,0, 0);
	normalOutput = vec4(bump * 0.5 + 0.5, 1);
}