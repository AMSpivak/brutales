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
		private:
			Resource m_buffer;
			TargetFormat m_format;
			DescriptorHandle m_descriptor_handle;
		public:
			DepthBuffer() = default;
			DepthBuffer(const DepthBuffer&) = default;
			~DepthBuffer() {};
			
			bool CreateSrv(Device& device, DescriptorHandle& descriptor_handle);
			void Assign(Device& device, int width, int height, TargetFormat format);
		};
	}
}

#endif
