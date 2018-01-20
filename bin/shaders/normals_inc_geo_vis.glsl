#version 430
layout ( triangles ) in;
layout ( line_strip, max_vertices = 8 ) out;


in vec3 triagColor[];
in vec4 light[];
in vec4 worldCoord[];
in vec3 normalOUT[];


out vec3 color;


void main()
{
	vec4 normalOrigin = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
	color=vec3(0,1,0);
	//output normal line
	gl_Position = normalOrigin;
	EmitVertex();
	gl_Position = normalOrigin + normalOUT[0];
	EmitVertex();
    EndPrimitive();

	//triagColor= colorTriag[0];
	gl_Position = gl_in[0].gl_Position;
	color=triagColor[0];
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	color=triagColor[1];
	EmitVertex();
    EndPrimitive();

	gl_Position = gl_in[1].gl_Position;
	color=triagColor[1];
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	color=triagColor[2];
	EmitVertex();
    EndPrimitive();

	gl_Position = gl_in[2].gl_Position;
	color=triagColor[2];
	EmitVertex();
	gl_Position = gl_in[0].gl_Position;
	color=triagColor[0];
	EmitVertex();
    EndPrimitive();
}