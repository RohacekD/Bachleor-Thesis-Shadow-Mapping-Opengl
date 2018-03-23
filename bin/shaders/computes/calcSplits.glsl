#version 430

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

// should be summed up to ~1.0f
uniform float splitRatios[4];

layout(std430, binding = 3) buffer histogram
{
    uint data_SSBO[256];
    uint highestValue; // 
    uint lowestIndex;
    uint highestIndex;
    uint numSamples;
};

//=================================================================================
layout(std430, binding = 4) buffer splitFrustums
{
    uvec2 frustum[4];
};

//=================================================================================
void main()
{
	frustum[0].x = lowestIndex;
	uint sum = 0;
	uint actualGroup = 0;
	uint groupTrashhold = uint(splitRatios[0] * numSamples);
	for(uint i = lowestIndex; i <= highestIndex && actualGroup<4; ++i){
		sum += data_SSBO[i];
		if(sum > groupTrashhold){
			frustum[actualGroup].y = i;
			frustum[actualGroup+1].x = i;
			++actualGroup;
			groupTrashhold = uint(splitRatios[actualGroup] * numSamples);
		}
	}
	frustum[3].y = highestIndex;
}