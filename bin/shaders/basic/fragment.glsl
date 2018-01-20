#version 430

//uniform sampler2D tex;
uniform sampler2D shadowMap;

varying vec3 normalOUT;
varying vec4 lightOUT;
varying vec4 toLight;
varying vec4 worldCoord;
//varying vec2 texCoordOUT;
varying vec4 shadowCoords;

out vec4 fragColor;


void main()
{
	float objectNearestLight = texture(shadowMap, shadowCoords.xy).r;
	float lightFactor = 1.0f;

	vec4 MaterialDiffuseColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	if(shadowCoords.z>objectNearestLight){
		MaterialDiffuseColor = vec4(0.0f,0.0f,0.0f,1.0f);
	}


	float lightDistance = length(toLight);
	vec4 ligthDirection = toLight;
	vec3 n = normalize( normalOUT );
	float cosTheta = clamp( dot( n,ligthDirection.xyz ), 0,1 );

	//vec4 MaterialDiffuseColor = texture(tex, texCoordOUT);

	

	vec4 LightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float LightPower = 150;
	fragColor = MaterialDiffuseColor;// * LightColor * LightPower * cosTheta / (lightDistance*lightDistance);
}