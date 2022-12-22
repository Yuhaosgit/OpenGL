#version 400 core

uniform sampler2D water_diffuseTex;
uniform sampler2D water_bumpTex;
uniform samplerCube water_cubeTex;

uniform vec4 lightColour;
uniform vec3 lightOrientation;
//uniform vec3 lightPos;
//uniform float lightRadius;

uniform vec3 cameraPos;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
} IN;

out vec4 accumulation;
out float reveal;

void main(){
	//lighting
	vec3 incident = -lightOrientation;
	vec3 view_direction = normalize(cameraPos - IN.worldPos);
	vec3 half_direction = normalize(incident + view_direction);

	mat3 TBN = mat3(normalize(IN.tangent),normalize(IN.binormal),normalize(IN.normal));

	vec4 diffuse = texture(water_diffuseTex, IN.texCoord);
	vec3 bump_normal = texture(water_bumpTex,IN.texCoord).rgb;
	bump_normal   = normalize(TBN * normalize(bump_normal * 2.0 - 1.0));

	//lighting
	vec3 lambert = clamp(dot(incident, bump_normal),0.0,1.0) * lightColour.rgb;
	
	float sepcularFac= clamp(dot(half_direction, bump_normal), 0.0, 1.0);
	vec3 specular = pow(sepcularFac, 30)* lightColour.rgb;

	vec3 surface = lambert * diffuse.xyz;
	surface += specular;

	//reflection for skybox
	vec3 reflectDir = reflect(-view_direction,normalize(IN.normal));
	vec4 reflectTex = texture(water_cubeTex,reflectDir);

	//weighted blender
	vec4 output_colour = vec4(surface, 0.3);
	if (specular.x > 0.85) {
		output_colour.a = 0.99;
	}

	float weight = clamp(pow(min(1.0, output_colour.a * 10.0) + 0.01, 3.0) * 1e8 *
		pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);

	accumulation = vec4(output_colour.rgb * output_colour.a, output_colour.a) * weight;
	reveal = output_colour.a;
}