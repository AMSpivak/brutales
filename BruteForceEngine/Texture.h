#ifndef BRUTEFORCE_TEXTURE_H
#define BRUTEFORCE_TEXTURE_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"
#include <mutex>
namespace BruteForce
{
	namespace Textures
	{
		class Texture
		{
		private:
			std::mutex m_mutex;
		public:
			Texture() = default;
			Texture(const Texture &) = default;
			~Texture() {};
			Resource image;
			TargetFormat Format;
			DescriptorHandle m_descriptor_handle;
			void CreateSrv(Device& device, DescriptorHandle& descriptor_handle);
		friend void LoadTextureFromFile(Texture&, const std::wstring& /*, TextureUsage textureUsage */, Device&, SmartCommandQueue&);
		};

		void LoadTextureFromFile(Texture& texture, const std::wstring& fileName/*, TextureUsage textureUsage */, Device& device, SmartCommandQueue& smart_queue);
	}
}

#endif

