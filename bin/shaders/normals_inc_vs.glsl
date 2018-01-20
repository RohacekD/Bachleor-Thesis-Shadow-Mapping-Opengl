#version 330

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
out vec3 triagColor;
out vec4 light;
out vec4 worldCoord;
out vec3 normalOUT;
attribute vec3 vertex;
attribute vec3 normal;

void main()
{
	light = projectionMatrix * viewMatrix * modelMatrix * vec4(0.0f,5.0f,0.0f,1.0f);
	worldCoord = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.0f);

	triagColor=vec3(1,0,0);
	normalOUT = normal;
    gl_Position = worldCoord;
}