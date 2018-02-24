#version 430

#define PSSM_SPLITS 4

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

//================================================================================
uniform PSSM{
	float[PSSM_SPLITS] PSSM_Limits;
	mat4 PSSM_CameraView;
	mat4 PSSM_CameraProjection;
	mat4[PSSM_SPLITS] m_LightViewProjection;
} pssm;

//================================================================================
void main()
{
	for(int j = 0; j < gl_in.length(); ++j){
		gl_Layer = gl_InvocationID;
		gl_Position = pssm.m_LightViewProjection[gl_InvocationID] * gl_in[j].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}