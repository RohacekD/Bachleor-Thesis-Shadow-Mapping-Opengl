#version 330

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
varying  vec3 colorTriag;
attribute vec3 vertex;

void main()
{
	colorTriag=vec3(1,0,0);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0);
}