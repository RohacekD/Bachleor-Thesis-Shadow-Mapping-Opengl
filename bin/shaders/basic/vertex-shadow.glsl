#version 330

attribute vec4 vertex;
attribute vec3 normal;

//per frame
uniform mat4 projectionMatrix;

//per vertex
uniform mat4 modelMatrix;

void main()
{
    gl_Position = modelMatrix * (vertex);
}