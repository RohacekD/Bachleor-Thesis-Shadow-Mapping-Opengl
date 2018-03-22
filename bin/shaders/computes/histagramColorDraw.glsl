#version 430
layout(local_size_x = 1, local_size_y = 150, local_size_z = 1) in;

layout (binding = 0, rgba32f) writeonly uniform image2D histagramTex;

layout(std430, binding = 3) buffer histogram
{
    int data_SSBO[256];
    int highestValue;
};


layout(std430, binding = 4) buffer splitFrustums
{
    uvec2 frustum[4];
};

vec4 frustColors[5] = {
	vec4(1.0, 0.0, 0.0, 1.0),
	vec4(0.0, 1.0, 0.0, 1.0),
	vec4(0.0, 0.0, 1.0, 1.0),
	vec4(0.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 0.0, 1.0)
};

bool inFrustum(uvec2 frustum, uint pos){
	if(pos > frustum.x && pos < frustum.y){
		return true;
	}
	return false;
}

bool isBorder(uvec2 frustum, uint pos){
	if(pos == frustum.x || pos == frustum.y){
		return true;
	}
	return false;
}

vec4 getColorForFrustum(uint pos){
	for(int i = 0; i < 4; ++i){
		if(inFrustum(frustum[i], pos)){
			return frustColors[i];
		}
	}
	return frustColors[4];
}

void main()
{
	uint posX = gl_GlobalInvocationID.x;
	uint posY = gl_GlobalInvocationID.y;

	for(int i = 0; i < 4; ++i){
		if(isBorder(frustum[i], posX)){
			imageStore(histagramTex, ivec2(posX, posY), vec4(0, 0, 0, 1));
			return;
		}
	}
	
	if(highestValue!=0 && posY < float(gl_WorkGroupSize.y) / float(highestValue)  * float(data_SSBO[posX])){
		imageStore(histagramTex, ivec2(posX, posY), getColorForFrustum(posX));
	}	
	else{
		imageStore(histagramTex, ivec2(posX, posY), vec4(1, 1, 1, 1));
	}
}