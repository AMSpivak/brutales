#ifndef BRUTEFORCE_TEXTURE_H
#define BRUTEFORCE_TEXTURE_H
//#include "PlatformDefine.h"
#include "TexturePlat.h"
#include "Resources.h"
#include <mutex>
#include <vector>
#include <memory>

namespace BruteForce
{
	namespace Textures
	{
		struct TextureLoadHlpr
		{
			Device& m_device;
			SmartCommandQueue& m_copy_queue;
			GpuAllocator m_gpu_allocator;
			TextureLoadHlpr(Device& device, SmartCommandQueue& copy_queue, GpuAllocator gpu_allocator):m_device(device), m_copy_queue(copy_queue), m_gpu_allocator(gpu_allocator){}
		};


		class Texture : public GpuResource
		{
		private:
			std::mutex m_mutex;
			size_t m_Mips;
			size_t m_Width;
			size_t m_Height;

			DescriptorHandle    m_rtvDescriptor;
			float               m_clearColor[4];
			bool				m_render_target;
		public:
			Texture() : m_render_target(false) {};
			Texture(const std::wstring& filename, TextureLoadHlpr& helper, DescriptorHandle& p_srv_handle_start, TargetFormat format = TargetFormat_Unknown);
			Texture(const std::wstring& filename, TextureLoadHlpr& helper, DescriptorHeapRange& descriptor_range, size_t index, TargetFormat format = TargetFormat_Unknown);

			Texture(const Texture&) = default;
			~Texture()
			{};

			TargetFormat m_format;
			
			void CreateSrv(Device& device, DescriptorHandle& descriptor_handle);
			void CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index);
			void CreateUav(Device& device, DescriptorHandle& descriptor_handle);
			void CreateUav(Device& device, DescriptorHeapRange& descriptor_range, size_t index);
			void CreateRtv(Device& device, DescriptorHandle& rt_handle);
			void CreateDsv(Device& device, DescriptorHandle& rt_handle);

			DescriptorHandle& GetRT();

			size_t GetWidth() { return m_Width; }
			size_t GetHeight() { return m_Height; }

		friend void LoadTextureFromFile(Texture&, const std::wstring& /*, TextureUsage textureUsage */, TextureLoadHlpr&);
		friend void CreateTexture(Texture& texture, const TexMetadata& metadata, Device& device, bool render_target, bool is_uav, bool is_depth, GpuAllocator gpu_allocator);
		};

		void CreateTexture(Texture& texture, const TexMetadata& metadata, Device& device, bool render_target, bool is_uav, bool is_depth, GpuAllocator gpu_allocator = nullptr);

		void LoadTextureFromFile(Texture& texture, const std::wstring& fileName/*, TextureUsage textureUsage */, TextureLoadHlpr& helper);

		void AddTexture(const std::wstring& content_path, const std::wstring& filename, std::vector<std::shared_ptr<Texture>>& textures, TextureLoadHlpr& helper, DescriptorHandle& p_srv_handle_start, TargetFormat format = TargetFormat_Unknown);
		
		template <typename T>
		void AddTextures(T i_start, T i_end, const std::wstring& content_path, std::vector<std::shared_ptr<Texture>>& textures, Device& device, DescriptorHandle& p_srv_handle_start, GpuAllocator gpu_allocator = nullptr)
		{
			SmartCommandQueue copy_queue(device, BruteForce::CommandListTypeCopy);
			TextureLoadHlpr helper { device, copy_queue, gpu_allocator };
			while(i_start != i_end)
			{
				AddTexture(content_path, (*i_start), textures, helper, p_srv_handle_start);
				++i_start;
			}
		}

		template <typename T>
		void AddTextures(T i_start, T i_end, const std::wstring& content_path, std::vector<std::shared_ptr<Texture>>& textures, TextureLoadHlpr& helper,  DescriptorHandle& p_srv_handle_start)
		{
			while (i_start != i_end)
			{
				AddTexture(content_path, (*i_start), textures, helper, p_srv_handle_start);
				++i_start;
			}
		}
	}
}

#endif

