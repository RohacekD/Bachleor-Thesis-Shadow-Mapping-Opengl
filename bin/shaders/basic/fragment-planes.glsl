#version 430

#define PSSM_SPLITS 4

uniform sampler2D tex;
uniform sampler2DArray shadowMap;

in vec3 normalOUT;
in vec4 lightOUT;
in vec4 toLight;
in vec4 worldCoord;
varying vec2 texCoordOUT;
in vec4 shadowCoords;
in float[PSSM_SPLITS] limits;
in vec4 camPosition;

in vec4 PSSM_CameraDependentPos;

in vec4 gl_FragCoord;;

out vec4 fragColor;

vec4 MaterialDiffuseColor;

int PSSMPlane(){
	float linDepth = PSSM_CameraDependentPos.z;

	if(linDepth < limits[0]){
		MaterialDiffuseColor += vec4(1.0, 0.0, 0.0, 1.0)/5;
		return 0;
	}
	else if(linDepth < limits[1]){
		MaterialDiffuseColor += vec4(0.0, 1.0, 0.0, 1.0)/5;
		return 1;
	}
	else if(linDepth < limits[2]){
		MaterialDiffuseColor += vec4(0.0, 0.0, 1.0, 1.0)/5;
		return 2;
	} 
		return 3;
	}
	else{
		MaterialDiffuseColor += vec4(1.0, 1.0, 0.0, 1.0)/5;
		return 4;
	}
}

bool IsInPSSMShadow(){
	int split = PSSMPlane();
	return split < 3;
}

void main()
{
	float objectNearestLight = texture(shadowMap, vec3(shadowCoords.xy, 0)).r;

	MaterialDiffuseColor = texture(tex, texCoordOUT);

	float zfar = 7.59345436/3.0;
	IsInPSSMShadow();

	if(shadowCoords.z - (1.0f/1024.0f)>objectNearestLight){
		MaterialDiffuseColor *= 0.1f;
	}

	fragColor = MaterialDiffuseColor;
}