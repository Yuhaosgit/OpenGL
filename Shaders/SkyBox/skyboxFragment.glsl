#version 330 core

uniform samplerCube cubeTex;

in Vertex {
	vec3 viewDir;
} IN;

out vec4 fragcolour;

void main(){
	fragcolour = texture(cubeTex, normalize(IN.viewDir));
	//gl_FragDepth = 1.000000f;
}