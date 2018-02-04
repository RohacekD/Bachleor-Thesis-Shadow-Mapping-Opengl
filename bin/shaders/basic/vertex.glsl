#version 330

attribute vec4 vertex;
attribute vec3 normal;
//attribute vec2 texCoord;

//per frame
uniform mat4 toShadowMapSpaceMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lightPos;
uniform vec4 PSSMLimits;
uniform vec4 CameraPosition;

//per vertex
uniform mat4 modelMatrix;

out vec3 normalOUT;
out vec4 lightOUT;
out vec4 toLight;
//out vec2 texCoordOUT;
out vec4 worldCoord;
out vec4 shadowCoords;
out vec4 limits;
out vec4 camPosition;

void main()
{
	normalOUT = normal;
	limits = PSSMLimits;
	camPosition = CameraPosition;
	//texCoordOUT = texCoord;

	//to light should be counted from world space

	shadowCoords = toShadowMapSpaceMatrix * vertex;// so far I don't have model matrix

	toLight = normalize(lightPos - vertex);
	worldCoord = modelMatrix * vertex;
	//camDistance = distance(vertex, CameraPosition);
    gl_Position = projectionMatrix * worldCoord;
}