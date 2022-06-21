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
            SrvDesc shaderResourceViewDesc = {};
            shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            shaderResourceViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            shaderResourceViewDesc.Format = Format;
            shaderResourceViewDesc.Texture2D.MipLevels = static_cast<UINT>(m_Mips);
            shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
            shaderResourceViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
            device->CreateShaderResourceView(image.Get(), &shaderResourceViewDesc, descriptor_handle);
            //m_descriptor_handle = descriptor_handle;
        }

        void Texture::CreateUav(Device& device, DescriptorHandle& descriptor_handle)
        {
            D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.Format = Format;
            uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = 0;
            uavDesc.Texture2D.PlaneSlice = 0;
            device->CreateUnorderedAccessView(image.Get(), nullptr, &uavDesc, descriptor_handle);
            //m_descriptor_handle = descriptor_handle;
        }

        void Texture::CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index)
        {
            assert(index < descriptor_range.m_Size);
            m_srv_index = index;
            auto descriptor_handle = descriptor_range.m_CpuHandle;
            descriptor_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav) * m_srv_index;
            CreateSrv(device, descriptor_handle);
        }

        void LoadTextureFromFile(Texture& texture, const std::wstring& fileName/*, TextureUsage textureUsage */, Device& device, SmartCommandQueue& smart_queue)
        {
            std::filesystem::path filePath(fileName);
            if (!std::filesystem::exists(filePath))
            {
                throw std::exception("File not found.");
            }

            std::lock_guard<std::mutex> lock(texture.m_mutex);

            {
                DirectX::TexMetadata metadata;
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
                ResourceDesc textureDesc = {};
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
                    break;
                case DirectX::TEX_DIMENSION_TEXTURE3D:
                    textureDesc = CD3DX12_RESOURCE_DESC::Tex3D(
                        metadata.format,
                        static_cast<UINT64>(metadata.width),
                        static_cast<UINT>(metadata.height),
                        static_cast<UINT16>(metadata.depth));
                    break;
                default:
                    throw std::exception("Invalid texture dimension.");
                    break;
                }
                HeapProperties props(D3D12_HEAP_TYPE_DEFAULT);
                //Resource textureResource;
                ThrowIfFailed(device->CreateCommittedResource(
                    &props,
                    HeapFlagsNone,
                    &textureDesc,
                    ResourceStateCommon,
                    nullptr,
                    IID_PPV_ARGS(&texture.image)));

                //texture.image = textureResource;

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

                smart_queue.CopyTextureSubresource(
                    texture.image,
                    0,
                    static_cast<uint32_t>(subresources.size()),
                    subresources.data(), ResourceStateCommon);

                if (subresources.size() < texture.image->GetDesc().MipLevels)
                {
                    //GenerateMips(texture);
                }

                texture.Format = metadata.format;
                
            }


        }

        void AddTexture(const std::wstring& content_path, const std::wstring& filename, std::vector<std::shared_ptr<Texture>>& textures, Device& device, SmartCommandQueue& copy_queue, DescriptorHandle& p_srv_handle_start, TargetFormat format)
        {
            auto texture = textures.emplace_back(std::make_shared<BruteForce::Textures::Texture>());
            LoadTextureFromFile(*texture, (content_path + filename), device, copy_queue);
            texture->image->SetName((L"Texture: " + filename).c_str());

            if (format != DXGI_FORMAT_UNKNOWN)
            {
                texture->Format = format;
            }

            texture->CreateSrv(device, p_srv_handle_start);
            p_srv_handle_start.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
        }
    }
}
