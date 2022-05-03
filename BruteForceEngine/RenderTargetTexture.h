#ifndef BRUTEFORCE_RENDERTARGETTEXTURE_H
#define BRUTEFORCE_RENDERTARGETTEXTURE_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"

namespace BruteForce
{
	namespace Textures
	{
		class RenderTargetTexture
		{
		private:
			Resource              m_resource;
			ResourceStates                               m_state;
			D3D12_CPU_DESCRIPTOR_HANDLE                         m_srvDescriptor;
			D3D12_CPU_DESCRIPTOR_HANDLE                         m_rtvDescriptor;

			TargetFormat                                         m_format;

			size_t                                              m_width;
			size_t                                              m_height;
		public:

		};

	}
}

#endif
