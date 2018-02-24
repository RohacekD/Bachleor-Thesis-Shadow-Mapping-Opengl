#version 330

#define PSSM_SPLITS 4

attribute vec4 vertex;
attribute vec3 normal;
attribute vec2 texCoord;

//per frame
uniform mat4 projectionMatrix;
uniform vec4 CameraPosition;

//per model
uniform mat4 modelMatrix;


//=================================================================================
out vec4 PSSM_CameraDependentPos;

uniform PSSM{
	float[PSSM_SPLITS] PSSM_Limits;
	mat4 PSSM_CameraView;
	mat4 PSSM_CameraProjection;
	mat4[PSSM_SPLITS] m_LightViewProjection;
} pssm;

//=================================================================================
out vec3 normalOUT;
out vec4 lightOUT;
out vec4 toLight;
out vec2 texCoordOUT;
out vec4 worldCoord;
out vec4 camPosition;


//=================================================================================
void main()
{
	normalOUT = normal;
	camPosition = pssm.PSSM_CameraView * CameraPosition;
	texCoordOUT = texCoord;

	//to light should be counted from world space

	//shadowCoords = toShadowMapSpaceMatrix * modelMatrix * vertex;// so far I don't have model matrix

	worldCoord = modelMatrix * vertex;

    gl_Position = projectionMatrix * worldCoord; //tohle je reálná hloubka
    PSSM_CameraDependentPos = pssm.PSSM_CameraView * worldCoord; //tohle je na odvozeni hloubek
}