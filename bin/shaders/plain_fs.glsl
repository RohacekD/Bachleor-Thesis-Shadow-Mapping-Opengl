#version 430

out vec4 fragColor;

in vec3 colorTriag;

void main()
{
   fragColor = vec4(colorTriag, 0.6f);
}