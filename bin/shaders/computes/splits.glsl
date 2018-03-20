#version 430

layout (binding = 0) uniform sampler2D depthBuffer;


layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

uniform ivec2 globalSize;

layout(std430, binding = 3) buffer histogram
{
    uint data_SSBO[256];
    uint highestValue;
};

void main()
{
	vec2 position = ivec2(gl_GlobalInvocationID.xy);
	if(position.x < globalSize.x && position.y < globalSize.y){
		ivec2       position = ivec2(gl_GlobalInvocationID.xy);
		vec2        screenNormalized = vec2(position) / vec2(globalSize); // ctxSize is the size of the depth and color textures
		vec4        depthSample = texture(depthBuffer, screenNormalized);
		if(depthSample.r==1.0) return;
		int 		index = int(depthSample.r*255);
		atomicAdd(data_SSBO[index], 1);
		atomicMax(highestValue, data_SSBO[index]);
	}
}