#include "Texture.h"
#include "DirectXTex.h"
#include "PlatformDefine.h"
#include "Helpers.h"
#include  <filesystem>
#include  <mutex>

namespace BruteForce
{
    namespace Textures
    {
        void Texture::CreateSrv(Device& device, DescriptorHandle& descriptor_handle)
        {
            if (m_render_target)
            {
                device->CreateShaderResourceView(m_GpuBuffer.Get(), nullptr, descriptor_handle);
                return;
            }
            SrvDesc shaderResourceViewDesc = {};
            shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            if (m_format != TargetFormat_R32_Typeless)
            {
                shaderResourceViewDesc.Format = m_format;
            }
            else
            {
                shaderResourceViewDesc.Format = TargetFormat_R32_Float;
            }

            shaderResourceViewDesc.Texture2D.MipLevels = static_cast<UINT>(m_Mips);
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
            device->CreateShaderResourceView(m_GpuBuffer.Get(), &shaderResourceViewDesc, descriptor_handle);
            //m_descriptor_handle = descriptor_handle;
        }

        void Texture::CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index)
        {
            assert(index < descriptor_range.m_Size);
            m_heap_range_srv_index = index;
            auto descriptor_handle = descriptor_range.m_CpuHandle;
            descriptor_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav) * m_heap_range_srv_index;
            CreateSrv(device, descriptor_handle);
        }

        void Texture::CreateUav(Device& device, DescriptorHandle& descriptor_handle)
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = m_format;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = 0;
            uavDesc.Texture2D.PlaneSlice = 0;
            device->CreateUnorderedAccessView(m_GpuBuffer.Get(), nullptr, &uavDesc, descriptor_handle);
            //m_descriptor_handle = descriptor_handle;
        }

        void Texture::CreateUav(Device& device, DescriptorHeapRange& descriptor_range, size_t index)
        {
            assert(index < descriptor_range.m_Size);
            m_heap_range_uav_index = index;
            auto descriptor_handle = descriptor_range.m_CpuHandle;
            descriptor_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav) * m_heap_range_uav_index;
            CreateUav(device, descriptor_handle);
        }

        void Texture::CreateRtv(Device& device, DescriptorHandle& rt_handle)
        {
            if (!m_GpuBuffer)
            {
                return;
            }

            device->CreateRenderTargetView(m_GpuBuffer.Get(), nullptr, rt_handle);
            m_rtvDescriptor = rt_handle;
        }

        void Texture::CreateDsv(Device& device, DescriptorHandle& dsv_handle)
        {
            if (!m_GpuBuffer)
            {
                return;
            }

            D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
            dsv.Format = m_format;
            dsv.Format = TargetFormat_D32_Float;

            dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsv.Texture2D.MipSlice = 0;
            dsv.Flags = D3D12_DSV_FLAG_NONE;

            device->CreateDepthStencilView(m_GpuBuffer.Get(), &dsv,
                dsv_handle);
            
        }

        DescriptorHandle& Texture::GetRT()
        {
            return m_rtvDescriptor;
        }

        bool FillTextureDescriptor(const TexMetadata& metadata, ResourceDesc& textureDesc)
        {
            switch (metadata.dimension)
            {
            case DirectX::TEX_DIMENSION_TEXTURE1D:
                textureDesc = CD3DX12_RESOURCE_DESC::Tex1D(
                    metadata.format,
                    static_cast<UINT64>(metadata.width),
                    static_cast<UINT16>(metadata.arraySize));
                break;
            case DirectX::TEX_DIMENSION_TEXTURE2D:
                textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
                    metadata.format,
                    static_cast<UINT64>(metadata.width),
                    static_cast<UINT>(metadata.height),
                    static_cast<UINT16>(metadata.arraySize));
                textureDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
                break;
            case DirectX::TEX_DIMENSION_TEXTURE3D:
                textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(
                    metadata.format,
                    static_cast<UINT64>(metadata.width),
                    static_cast<UINT>(metadata.height),
                    static_cast<UINT16>(metadata.depth));
                break;
            default:
                    //throw std::exception("Invalid texture dimension.");
                    return false;
                break;
            }
            return true;
        }

        void CreateTexture(Texture& texture, const TexMetadata& metadata, Device& device, bool render_target, bool is_uav, bool is_depth, GpuAllocator gpu_allocator)
        {
            texture.m_render_target = render_target;
            ResourceDesc textureDesc = {};
            if (render_target)
            {
                TexMetadata tmp_meta(metadata);
                tmp_meta.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
                tmp_meta.arraySize = 1;
                tmp_meta.mipLevels = 1;
                FillTextureDescriptor(tmp_meta, textureDesc);
                texture.m_Mips = 1;
            }
            else if (is_depth)
            {
                TexMetadata tmp_meta(metadata);
                tmp_meta.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
                tmp_meta.arraySize = 1;
                tmp_meta.mipLevels = 1;
                tmp_meta.format = TargetFormat_R32_Typeless;
                FillTextureDescriptor(tmp_meta, textureDesc);
                texture.m_Mips = 1;
            }
            else
            {
                FillTextureDescriptor(metadata, textureDesc);
                texture.m_Mips = metadata.mipLevels;
            }

            //Resource textureResource;

            D3D12_CLEAR_VALUE* pClearValue = nullptr;
            texture.m_state = ResourceStateCommon;
            textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            D3D12_CLEAR_VALUE clearValue = { metadata.format, {} };

            if (render_target)
            {
                texture.m_clearColor[0] = texture.m_clearColor[1] = texture.m_clearColor[2] = texture.m_clearColor[3] = 0.0f;

                pClearValue = &clearValue;

                textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                //texture.m_state = ResourceStatesRenderTarget;
            }
            if (is_uav)
            {
                textureDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
                //pClearValue = nullptr;
            }

            if (is_depth)
            {
                assert(!(is_uav||render_target));
                textureDesc.Flags = ResourceFlagsDepthStencil;
                clearValue.Format = TargetFormat_D32_Float;
                clearValue.DepthStencil = { 1.0f, 0 };
                pClearValue = &clearValue;
            }
 
            CreateBufferResource(device, texture, &textureDesc, pClearValue, gpu_allocator);
            texture.m_Width = metadata.width;
            texture.m_Height = metadata.height;
            texture.m_format = metadata.format;
        }

        void LoadTextureFromFile(Texture& texture, const std::wstring& fileName/*, TextureUsage textureUsage */, TextureLoadHlpr& helper)
        {
            std::filesystem::path filePath(fileName);
            if (!std::filesystem::exists(filePath))
            {
                throw std::exception("File not found.");
            }

            std::lock_guard<std::mutex> lock(texture.m_mutex);

            {
                TexMetadata metadata;
                DirectX::ScratchImage scratchImage;

                if (filePath.extension() == ".dds")
                {
                    ThrowIfFailed(LoadFromDDSFile(
                        fileName.c_str(),
                        DirectX::DDS_FLAGS_FORCE_RGB,
                        &metadata,
                        scratchImage));
                }
                else if (filePath.extension() == ".hdr")
                {
                    ThrowIfFailed(LoadFromHDRFile(
                        fileName.c_str(),
                        &metadata,
                        scratchImage));
                }
                else if (filePath.extension() == ".tga")
                {
                    ThrowIfFailed(LoadFromTGAFile(
                        fileName.c_str(),
                        &metadata,
                        scratchImage));
                }
                else
                {
                    ThrowIfFailed(LoadFromWICFile(
                        fileName.c_str(),
                        DirectX::WIC_FLAGS_FORCE_RGB,
                        &metadata,
                        scratchImage));
                }

                /*if (textureUsage == TextureUsage::Albedo)
                {
                    metadata.format = MakeSRGB(metadata.format);
                }*/

                CreateTexture(texture, metadata, helper.m_device, false, false, false, helper.m_gpu_allocator);

                std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
                const DirectX::Image* pImages = scratchImage.GetImages();
                for (int i = 0; i < scratchImage.GetImageCount(); ++i)
                {
                    auto& subresource = subresources[i];
                    subresource.RowPitch = pImages[i].rowPitch;
                    subresource.SlicePitch = pImages[i].slicePitch;
                    subresource.pData = pImages[i].pixels;
                }
                texture.m_Mips = scratchImage.GetImageCount();

                helper.m_copy_queue.CopyTextureSubresource(
                    texture.m_GpuBuffer,
                    0,
                    static_cast<uint32_t>(subresources.size()),
                    subresources.data(), ResourceStateCommon);

                if (subresources.size() < texture.m_GpuBuffer->GetDesc().MipLevels)
                {
                    //GenerateMips(texture);
                }

                texture.m_format = metadata.format;
                
            }


        }

        void AddTexture(const std::wstring& content_path, const std::wstring& filename, std::vector<std::shared_ptr<Texture>>& textures, TextureLoadHlpr& helper, DescriptorHandle& p_srv_handle_start, TargetFormat format)
        {
            auto texture = textures.emplace_back(std::make_shared<BruteForce::Textures::Texture>());
            LoadTextureFromFile(*texture, (content_path + filename), helper);
            texture->SetName((L"Texture: " + filename).c_str());

            if (format != TargetFormat_Unknown)
            {
                texture->m_format = format;
            }

            texture->CreateSrv(helper.m_device, p_srv_handle_start);
            p_srv_handle_start.ptr += helper.m_device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
        }
    }
}
