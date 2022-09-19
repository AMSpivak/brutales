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
                device->CreateShaderResourceView(m_resource.Get(), nullptr, descriptor_handle);
                return;
            }
            SrvDesc shaderResourceViewDesc = {};
            shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            shaderResourceViewDesc.Format = m_format;
            shaderResourceViewDesc.Texture2D.MipLevels = static_cast<UINT>(m_Mips);
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
            device->CreateShaderResourceView(m_resource.Get(), &shaderResourceViewDesc, descriptor_handle);
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
            device->CreateUnorderedAccessView(m_resource.Get(), nullptr, &uavDesc, descriptor_handle);
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
            if (!m_resource)
            {
                return;
            }

            device->CreateRenderTargetView(m_resource.Get(), nullptr, rt_handle);
            m_rtvDescriptor = rt_handle;
        }

        void Texture::SetName(LPCWSTR name)
        {
            m_resource->SetName(name);
        }

        void Texture::TransitionTo(SmartCommandList& commandlist, ResourceStates dst)
        {
            if (m_state == dst)
            {
                return;
            }

            ResourceBarrier barrier = BruteForce::ResourceBarrier::Transition(
                m_resource.Get(),
                m_state,
                dst);

            commandlist.command_list->ResourceBarrier(1, &barrier);

            m_state = dst;
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

        void CreateTexture(Texture& texture, const TexMetadata& metadata, Device& device, bool render_target, bool is_uav, GpuAllocator gpu_allocator)
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
            else
            {
                FillTextureDescriptor(metadata, textureDesc);
                texture.m_Mips = metadata.mipLevels;
            }

            HeapProperties props(D3D12_HEAP_TYPE_DEFAULT);
            //Resource textureResource;

            D3D12_CLEAR_VALUE* pClearValue = nullptr;
            texture.m_state = ResourceStateCommon;
            
            if (render_target)
            {
                texture.m_clearColor[0] = texture.m_clearColor[1] = texture.m_clearColor[2] = texture.m_clearColor[3] = 0.0f;

                D3D12_CLEAR_VALUE clearValue = { metadata.format, {} };
                memcpy(clearValue.Color, texture.m_clearColor, sizeof(clearValue.Color));
                pClearValue = &clearValue;

                textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                texture.m_state = ResourceStatesRenderTarget;
            }
            if (is_uav)
            {
                textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
                //pClearValue = nullptr;
            }
 
            if (gpu_allocator )
            {
                D3D12MA::ALLOCATION_DESC allocDesc = {};
                allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

                ThrowIfFailed(gpu_allocator->CreateResource(
                    &allocDesc, 
                    &textureDesc,
                    texture.m_state,
                    pClearValue,
                    &texture.m_p_allocation,
                    IID_PPV_ARGS(&texture.m_resource)));
            }
            else
            {
                ThrowIfFailed(device->CreateCommittedResource(
                    &props,
                    HeapFlagsNone,
                    &textureDesc,
                    texture.m_state,
                    pClearValue,
                    IID_PPV_ARGS(&texture.m_resource)));
            }
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

                CreateTexture(texture, metadata, helper.m_device, false, false, helper.m_gpu_allocator);

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
                    texture.m_resource,
                    0,
                    static_cast<uint32_t>(subresources.size()),
                    subresources.data(), ResourceStateCommon);

                if (subresources.size() < texture.m_resource->GetDesc().MipLevels)
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
