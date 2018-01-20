#version 430

out vec4 fragColor;

varying vec3 triagColor;
varying vec4 light;
varying vec4 worldCoord;
varying vec3 normalOUT;
float lightDistance;
uniform mat4 viewMatrix;

vec3 normal;

void main()
{
	normal = normalOUT;
	lightDistance = distance(light, worldCoord);
	vec4 ligthDirection = normalize(light - worldCoord);
	float cosTheta = clamp( dot( normal,ligthDirection.xyz ), 0,1 );
	float lightPower = 150;

	//specular
	vec4 eyeVector = normalize(vec4(0,0,0,1) - worldCoord);
	vec3 reflectDirection = reflect(-ligthDirection.xyz,normal);

	float cosAlpha = clamp( dot( eyeVector.xyz,reflectDirection ), 0,1 );

	vec3 lightColor = vec3(1,1,1);

	vec3 color = triagColor * lightColor * lightPower * cosTheta / (lightDistance*lightDistance)+
	vec3(1,1,1) * lightColor * lightPower * pow(cosAlpha,5) / (lightDistance*lightDistance);
	fragColor = vec4(color, 1.0f);
}