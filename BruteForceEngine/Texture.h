#ifndef BRUTEFORCE_TEXTURE_H
#define BRUTEFORCE_TEXTURE_H
//#include "PlatformDefine.h"
#include "PlatformTexture.h"
#include "EngineGpuCommands.h"
#include "DescriptorHeapManager.h"
#include <mutex>
#include <vector>
#include <memory>

namespace BruteForce
{
	namespace Textures
	{
		class Texture
		{
		private:
			std::mutex m_mutex;
			size_t m_Mips;
			size_t heap_range_srv_index;
			size_t heap_range_uav_index;
		public:
			Texture() = default;
			Texture(const Texture &) = default;
			~Texture() {};
			Resource image;
			TargetFormat Format;
			
			//DescriptorHandle m_descriptor_handle;
			void CreateSrv(Device& device, DescriptorHandle& descriptor_handle);
			void CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index);
			void CreateUav(Device& device, DescriptorHandle& descriptor_handle);
			void CreateUav(Device& device, DescriptorHeapRange& descriptor_range, size_t index);

		friend void LoadTextureFromFile(Texture&, const std::wstring& /*, TextureUsage textureUsage */, Device&, SmartCommandQueue&);
		friend void CreateTexture(Texture& texture, const TexMetadata& metadata, Device& device);
		};

		void CreateTexture(Texture& texture, const TexMetadata& metadata, Device& device);

		void LoadTextureFromFile(Texture& texture, const std::wstring& fileName/*, TextureUsage textureUsage */, Device& device, SmartCommandQueue& smart_queue);

		void AddTexture(const std::wstring& content_path, const std::wstring& filename, std::vector<std::shared_ptr<Texture>>& textures, Device& device, SmartCommandQueue& copy_queue, DescriptorHandle& p_srv_handle_start, TargetFormat format = TargetFormat_Unknown);
		
		template <typename T>
		void AddTextures(T i_start, T i_end, const std::wstring& content_path, std::vector<std::shared_ptr<Texture>>& textures, Device& device, DescriptorHandle& p_srv_handle_start)
		{
			SmartCommandQueue copy_queue(device, BruteForce::CommandListTypeCopy);

			while(i_start != i_end)
			{
				AddTexture(content_path, (*i_start), textures, device, copy_queue, p_srv_handle_start);
				++i_start;
			}
		}

		template <typename T>
		void AddTextures(T i_start, T i_end, const std::wstring& content_path, std::vector<std::shared_ptr<Texture>>& textures, Device& device, SmartCommandQueue& copy_queue, DescriptorHandle& p_srv_handle_start)
		{
			while (i_start != i_end)
			{
				AddTexture(content_path, (*i_start), textures, device, copy_queue, p_srv_handle_start);
				++i_start;
			}
		}
	}
}

#endif

