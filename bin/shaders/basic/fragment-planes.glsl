#version 430

#define PSSM_SPLITS 4

#include "include/splitFrustrum.glsl"
#include "include/pssmUniform.glsl"

uniform sampler2D tex;
uniform sampler2DArray shadowMap;

//per mesh
uniform bool hasTexture;
uniform vec4 modelColor;

in vec3 normalOUT;
in vec4 lightOUT;
in vec4 toLight;
in vec4 worldCoord;
varying vec2 texCoordOUT;
in vec4 camPosition;


in vec4 gl_FragCoord;;

out vec4 fragColor;

vec4 MaterialDiffuseColor;

//================================================================================
in vec4 PSSM_CameraDependentPos;

//=================================================================================
bool IsInPSSMShadow(){
	int split = getNumberFrustum(abs(PSSM_CameraDependentPos.z));
	vec4 shadowCoords = pssm.m_LightViewProjection[split] * worldCoord;
	shadowCoords = vec4(shadowCoords.xyz * 0.5 + 0.5, 1.0f);
	float objectNearestLight = texture(shadowMap, vec3(shadowCoords.xy, split)).r;
	return (shadowCoords.z - 1.0/1024.0 > objectNearestLight);
}

//=================================================================================
void ApplyPSSM(){
	if(IsInPSSMShadow()){
		MaterialDiffuseColor -= vec4(0.1,0.1,0.1,1);
	}
}

//=================================================================================
void main()
{
	if(hasTexture){
		MaterialDiffuseColor = texture(tex, texCoordOUT);
	}
	else{
		MaterialDiffuseColor = modelColor;
	}
	if(PSSM_CameraDependentPos.z<0.0)
		MaterialDiffuseColor += getColorForFrustum(abs(PSSM_CameraDependentPos.z))*0.3;

	ApplyPSSM();

	fragColor = MaterialDiffuseColor;
}