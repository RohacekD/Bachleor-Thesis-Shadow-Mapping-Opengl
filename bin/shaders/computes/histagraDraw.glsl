#version 430
layout(local_size_x = 1, local_size_y = 150, local_size_z = 1) in;

layout (binding = 0, rgba32f) writeonly uniform image2D histagramTex;

layout(std430, binding = 3) buffer histogram
{
    int data_SSBO[256];
    int highestValue;
};

void main()
{
	uint posX = gl_GlobalInvocationID.x;
	uint posY = gl_GlobalInvocationID.y;
	
	if(highestValue!=0 && posY < float(gl_WorkGroupSize.y) / float(highestValue)  * float(data_SSBO[posX])){
		imageStore(histagramTex, ivec2(posX, posY), vec4(0, 0, 0, 1));
	
}	else{
		imageStore(histagramTex, ivec2(posX, posY), vec4(1, 1, 1, 1));
	}
}