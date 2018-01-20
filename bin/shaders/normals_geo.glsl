#version 430
layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;

in vec3 colorTriag[];
in vec3 lights[];
out vec3 normal;
out vec3 triagColor;
out vec3 light;


void main()
{
	normal = normalize(cross(gl_in[0].gl_Position.xyz - gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz - gl_in[1].gl_Position.xyz));
	for(int i=0; i<gl_in.length();i++) {
		triagColor= colorTriag[i];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
    EndPrimitive();
}