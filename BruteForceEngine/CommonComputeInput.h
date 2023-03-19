#ifndef COMMON_COMPUTE_INPUT_H
#define COMMON_COMPUTE_INPUT_H
struct ComputeShaderInput
{
	uint3 GroupID : SV_GroupID;           // 3D index of the thread group in the dispatch.
	uint3 GroupThreadID : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
	uint3 DispatchThreadID : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
	uint  GroupIndex : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};
#endif
