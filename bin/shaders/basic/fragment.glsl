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

//per mesh
uniform bool hasTexture;
uniform vec4 modelColor;


in vec4 gl_FragCoord;

out vec4 fragColor;

vec4 MaterialDiffuseColor;



//=================================================================================
layout(std430, binding = 4) buffer splitFrustums
{
    uvec2 frustum[4];
};

//================================================================================
in vec4 PSSM_CameraDependentPos;

uniform PSSM{
	float[PSSM_SPLITS] PSSM_Limits;
	mat4 PSSM_CameraView;
	mat4 PSSM_CameraProjection;
	mat4[PSSM_SPLITS] m_LightViewProjection;
	vec3 SunDirection;
} pssm;

//=================================================================================
int PSSMPlane(){
	float linDepth = abs(PSSM_CameraDependentPos.z);

	if(linDepth < 0.0f){
		return 4;
	}
	else if(linDepth < 0.1f){
		return 4;
	}
	else if(linDepth < pssm.PSSM_Limits[0]){
		return 0;
	}
	else if(linDepth < pssm.PSSM_Limits[1]){
		return 1;
	}
	else if(linDepth < pssm.PSSM_Limits[2]){
		return 2;
	} 
	else if(linDepth < pssm.PSSM_Limits[3]){
		return 3;
	}
	else{
		return 4;
	}
}

//=================================================================================
bool IsInPSSMShadow(){
	int split = PSSMPlane();
	vec4 shadowCoords = pssm.m_LightViewProjection[split] * worldCoord;
	shadowCoords = vec4(shadowCoords.xyz * 0.5 + 0.5, 1.0f);
	float objectNearestLight = texture(shadowMap, vec3(shadowCoords.xy, split)).r;
	return (shadowCoords.z - (2.0f/1024.0f)  > objectNearestLight);
}

//=================================================================================
void ApplyPSSM(){
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

	if(IsInPSSMShadow()){
		MaterialDiffuseColor -= vec4(0.1,0.1,0.1,1);
	}
	else{
		vec3 n = normalize( normalOUT );
		float cosTheta = clamp( dot( n,-(pssm.SunDirection) ), 0.0,1.0 );

		vec4 LightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		float LightPower = 200.0;

		MaterialDiffuseColor = MaterialDiffuseColor * LightColor * cosTheta; 
	}

	fragColor = MaterialDiffuseColor;
}