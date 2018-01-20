#version 430

out vec4 fragColor;


in vec3 color;

vec3 normal;

void main()
{
	fragColor = vec4(color, 1.0f);
}