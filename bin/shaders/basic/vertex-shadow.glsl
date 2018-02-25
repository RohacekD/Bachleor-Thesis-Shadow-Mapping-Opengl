#version 330

attribute vec4 vertex;
attribute vec3 normal;

//per frame
uniform mat4 projectionMatrix;

//per vertex
uniform mat4 modelMatrix;

void main()
{
    gl_Position = modelMatrix * (vertex - vec4(normal, 1.0)*(3.0f/1024.0f));
}