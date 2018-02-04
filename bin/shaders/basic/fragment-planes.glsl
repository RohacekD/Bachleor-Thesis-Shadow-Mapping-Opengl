#version 430

//uniform sampler2D tex;
uniform sampler2D shadowMap;

in vec3 normalOUT;
in vec4 lightOUT;
in vec4 toLight;
in vec4 worldCoord;
//varying vec2 texCoordOUT;
in vec4 shadowCoords;
in vec4 limits;
in float camDistance;

in vec4 gl_FragCoord;;

out vec4 fragColor;


void main()
{
	float objectNearestLight = texture(shadowMap, shadowCoords.xy).r;
	float lightFactor = 1.0f;

	vec4 MaterialDiffuseColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);


	float lightDistance = length(toLight);
	vec4 ligthDirection = toLight;
	vec3 n = normalize( normalOUT );
	float cosTheta = clamp( dot( n,ligthDirection.xyz ), 0,1 );

	//vec4 MaterialDiffuseColor = texture(tex, texCoordOUT);

	float realZ = camDistance;

	if(realZ < limits.x){
		MaterialDiffuseColor = vec4(1.0, 0.0, 0.0, 1.0);
	}
	else if(realZ < limits.y){
		MaterialDiffuseColor = vec4(0.0, 1.0, 0.0, 1.0);
	}
	else if(realZ < limits.z){
		MaterialDiffuseColor = vec4(0.0, 0.0, 1.0, 1.0);
	}
	else if(realZ < limits.w){
		MaterialDiffuseColor = vec4(0.0, 1.0, 1.0, 1.0);
	}
	else{
		MaterialDiffuseColor = vec4(1.0, 1.0, 0.0, 1.0);
	}


	if(shadowCoords.z - (1.0f/1024.0f)>objectNearestLight){
		MaterialDiffuseColor *= 0.1f;
	}

	

	vec4 LightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float LightPower = 150;
	fragColor = MaterialDiffuseColor;// * LightColor * LightPower * cosTheta / (lightDistance*lightDistance);
}