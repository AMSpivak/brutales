#ifndef BRUTEFORCE_DEPTHBUFFER_H
#define BRUTEFORCE_DEPTHBUFFER_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"

namespace BruteForce
{
	namespace Textures
	{
		class DepthBuffer
		{
		public:
			DepthBuffer() = default;
			DepthBuffer(const DepthBuffer&) = default;
			~DepthBuffer() {};
			Resource m_buffer;
			TargetFormat m_format;
			DescriptorHandle m_descriptor_handle;
			void CreateSrv(Device& device, DescriptorHandle& descriptor_handle);
			void Assign(int width, int height, TargetFormat format, )
		};
	}
}

#endif
