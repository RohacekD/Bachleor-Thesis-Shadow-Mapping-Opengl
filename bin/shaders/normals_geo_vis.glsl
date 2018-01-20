#version 430
layout ( triangles ) in;
layout ( line_strip, max_vertices = 8 ) out;

in vec3 colorTriag[];
in vec3 lights[];
out vec3 normal;
out vec3 triagColor;
out vec3 light;


void main()
{
	normal = normalize(cross(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz));
	vec4 normalOrigin = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3;
	triagColor= vec3(0,1,0);
	gl_Position = normalOrigin;
	EmitVertex();
	gl_Position = normalOrigin + normal * 5.0f;
	EmitVertex();
    EndPrimitive();

	triagColor= colorTriag[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
    EndPrimitive();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
    EndPrimitive();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
    EndPrimitive();
}