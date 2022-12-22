#version 400 core

uniform mat4 shadowMatrix;
uniform mat4 modelMatrix;

in vec3 position;

void main(void)
{
	gl_Position = shadowMatrix * modelMatrix * vec4(position, 1.0);
}