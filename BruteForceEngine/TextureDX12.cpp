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
                Resource textureResource;
                ThrowIfFailed(device->CreateCommittedResource(
                    &props,
                    HeapFlagsNone,
                    &textureDesc,
                    ResourceStateCommon,
                    nullptr,
                    IID_PPV_ARGS(&textureResource)));

                texture.image = textureResource;

                std::vector<D3D12_SUBRESOURCE_DATA> subresources(scratchImage.GetImageCount());
                const DirectX::Image* pImages = scratchImage.GetImages();
                for (int i = 0; i < scratchImage.GetImageCount(); ++i)
                {
                    auto& subresource = subresources[i];
                    subresource.RowPitch = pImages[i].rowPitch;
                    subresource.SlicePitch = pImages[i].slicePitch;
                    subresource.pData = pImages[i].pixels;
                }

                smart_queue.CopyTextureSubresource(
                    texture.image,
                    0,
                    static_cast<uint32_t>(subresources.size()),
                    subresources.data());

                if (subresources.size() < textureResource->GetDesc().MipLevels)
                {
                    //GenerateMips(texture);
                }
                //ms_TextureCache[fileName] = textureResource.Get();
            }
        }
    }
}
