#version 430

#define PSSM_SPLITS 4

uniform sampler2D tex;
uniform sampler2DArray shadowMap;

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

uniform PSSM{
	float[PSSM_SPLITS] PSSM_Limits;
	mat4 PSSM_CameraView;
	mat4 PSSM_CameraProjection;
	mat4[PSSM_SPLITS] m_LightViewProjection;
} pssm;

//=================================================================================
int PSSMPlane(){
	float linDepth = abs(PSSM_CameraDependentPos.z - camPosition.z);

	if(linDepth < 0.0f){
		MaterialDiffuseColor += vec4(0.5, 0.5, 0.0, 1.0)/5;
		return 4;
	}
	else if(linDepth < 0.1f){
		MaterialDiffuseColor += vec4(1.0, 1.0, 1.0, 1.0);
		return 4;
	}
	else if(linDepth < pssm.PSSM_Limits[0]){
		MaterialDiffuseColor += vec4(1.0, 0.0, 0.0, 1.0)/5;
		return 0;
	}
	else if(linDepth < pssm.PSSM_Limits[1]){
		MaterialDiffuseColor += vec4(0.0, 1.0, 0.0, 1.0)/5;
		return 1;
	}
	else if(linDepth < pssm.PSSM_Limits[2]){
		MaterialDiffuseColor += vec4(0.0, 0.0, 1.0  , 1.0)/5;
		return 2;
	} 
	else if(linDepth < pssm.PSSM_Limits[3]){
		MaterialDiffuseColor += vec4(0.0, 1.0, 1.0, 1.0)/5;
		return 3;
	}
	else{
		MaterialDiffuseColor += vec4(1.0, 1.0, 0.0, 1.0)/5;
		return 4;
	}
}

//=================================================================================
bool IsInPSSMShadow(){
	int split = PSSMPlane();
	vec4 shadowCoords = pssm.m_LightViewProjection[split] * worldCoord;
	shadowCoords = vec4(shadowCoords.xyz * 0.5 + 0.5, 1.0f);
	float objectNearestLight = texture(shadowMap, vec3(shadowCoords.xy, split)).r;
	return (shadowCoords.z > objectNearestLight);
}

//=================================================================================
void ApplyPSSM(){
	if(IsInPSSMShadow()){
		MaterialDiffuseColor = vec4(0,0,0,1);
	}
}

//=================================================================================
void main()
{

	MaterialDiffuseColor = texture(tex, texCoordOUT);

	ApplyPSSM();

	fragColor = MaterialDiffuseColor;
}