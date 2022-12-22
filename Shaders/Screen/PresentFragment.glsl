#version 330 core

uniform sampler2D presentTex;

out vec4 fragment;

void main() {
	ivec2 coords = ivec2(gl_FragCoord.xy);
	fragment = texelFetch(presentTex, coords, 0);
}