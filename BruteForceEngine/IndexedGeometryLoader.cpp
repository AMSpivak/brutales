#include "IndexedGeometryGenerator.h"
#include "EngineGpuCommands.h"
#include "Resources.h"


#include <fstream>
#include <sstream>
#include <iostream>

#include <GLTFSDK/GLTF.h>
#include <GLTFSDK/GLTFResourceReader.h>
#include <GLTFSDK/GLBResourceReader.h>
#include <GLTFSDK/Deserialize.h>

namespace BruteForce
{
    namespace Geometry
    {

        void CreateGeometry(Device& device, IndexedGeometry& geometry, const float * vert_data, size_t vert_count, const WORD * ind_data, size_t ind_count)
        {

            BruteForce::CreateBufferResource(device, &geometry.m_VertexBuffer, vert_count, sizeof(BruteForce::VertexPosUvNormTangent));
            size_t vertex_size = sizeof(VertexPosUvNormTangent);
            SmartCommandQueue smart_queue(device, BruteForce::CommandListTypeDirect);
            auto commandList = smart_queue.GetCommandList();
            BruteForce::pResource intermediateVertexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &geometry.m_VertexBuffer, &intermediateVertexBuffer,
                vert_count, vertex_size, vert_data);

            geometry.m_VertexBufferView.BufferLocation = geometry.m_VertexBuffer->GetGPUVirtualAddress();
            geometry.m_VertexBufferView.SizeInBytes = vert_count * vertex_size;
            geometry.m_VertexBufferView.StrideInBytes = vertex_size;

            BruteForce::CreateBufferResource(device, &geometry.m_IndexBuffer, ind_count, sizeof(WORD));

            BruteForce::pResource intermediateIndexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &geometry.m_IndexBuffer, &intermediateIndexBuffer,
                ind_count, sizeof(WORD), ind_data);

            geometry.m_IndexBufferView.BufferLocation = geometry.m_IndexBuffer->GetGPUVirtualAddress();
            geometry.m_IndexBufferView.Format = TargetFormat_R16_UInt;
            geometry.m_IndexBufferView.SizeInBytes = ind_count * sizeof(WORD);


            auto fenceValue = smart_queue.ExecuteCommandList(commandList);
            smart_queue.WaitForFenceValue(fenceValue);
            geometry.m_IndexesCount = ind_count;
            intermediateIndexBuffer->Release();
            intermediateVertexBuffer->Release();
        }

        class StreamReader : public Microsoft::glTF::IStreamReader
        {
        public:
            StreamReader(std::filesystem::path pathBase) : m_pathBase(std::move(pathBase))
            {
                assert(m_pathBase.has_root_path());
            }

            // Resolves the relative URIs of any external resources declared in the glTF manifest
            std::shared_ptr<std::istream> GetInputStream(const std::string& filename) const override
            {
                // In order to construct a valid stream:
                // 1. The filename argument will be encoded as UTF-8 so use filesystem::u8path to
                //    correctly construct a path instance.
                // 2. Generate an absolute path by concatenating m_pathBase with the specified filename
                //    path. The filesystem::operator/ uses the platform's preferred directory separator
                //    if appropriate.
                // 3. Always open the file stream in binary mode. The glTF SDK will handle any text
                //    encoding issues for us.
                auto streamPath = m_pathBase / std::filesystem::u8path(filename);
                auto stream = std::make_shared<std::ifstream>(streamPath, std::ios_base::binary);
                
                // Check if the stream has no errors and is ready for I/O operations
                if (!stream || !(*stream))
                {
                    throw std::runtime_error("Unable to create a valid input stream for uri: " + filename);
                }

                return stream;
            }

        private:
            std::filesystem::path m_pathBase;
        };

        namespace GLTF
        {
            void PrintGltfInfo(const Microsoft::glTF::Document & document, const Microsoft::glTF::GLTFResourceReader & resourceReader)
            {
                using namespace Microsoft::glTF;
                // Use the resource reader to get each mesh primitive's position data
                for (const auto& mesh : document.meshes.Elements())
                {
                    std::cout << "Mesh: " << mesh.id << "\n";


                    for (const auto& meshPrimitive : mesh.primitives)
                    {
                        for (const auto& meshAttr : meshPrimitive.attributes)
                        {
                            std::cout << "MeshAttribs: " << meshAttr.first << " \n";
                        }

                        std::string accessorId;

                        if (meshPrimitive.TryGetAttributeAccessorId(ACCESSOR_POSITION, accessorId))
                        {
                            const Accessor& accessor = document.accessors.Get(accessorId);

                            const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
                            const auto dataByteLength = data.size() * sizeof(float);

                            std::cout << "MeshPrimitive: " << dataByteLength << " bytes of position data\n";
                        }
                    }
                }
            }

            bool FillGeometryFromMesh(Device& device, IndexedGeometry& geometry, const Microsoft::glTF::Document& document, const Microsoft::glTF::GLTFResourceReader& resourceReader)
            {
                using namespace Microsoft::glTF;
                const auto& mesh = document.meshes.Elements()[0];

                for (const auto& meshPrimitive : mesh.primitives)
                {
                    for (const auto& meshAttr : meshPrimitive.attributes)
                    {
                        std::cout << "MeshAttribs: " << meshAttr.first << " \n";
                    }

                    std::string accessorId;

                    std::vector<float> vPositions;
                    std::vector<float> vUV;
                    std::vector<float> vNormals;
                    std::vector<float> vTangent;
                    size_t num_vert = 0;

                    if (meshPrimitive.TryGetAttributeAccessorId(ACCESSOR_POSITION, accessorId))
                    {
                        const Accessor& accessor = document.accessors.Get(accessorId);
                        num_vert = accessor.count;

                        const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
                        vPositions = std::move(data);
                    }


                    if (meshPrimitive.TryGetAttributeAccessorId(ACCESSOR_TEXCOORD_0, accessorId))
                    {
                        const Accessor& accessor = document.accessors.Get(accessorId);

                        const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
                        vUV = std::move(data);
                    }

                    if (meshPrimitive.TryGetAttributeAccessorId(ACCESSOR_NORMAL, accessorId))
                    {
                        const Accessor& accessor = document.accessors.Get(accessorId);

                        const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
                        vNormals = std::move(data);
                    }

                    if (meshPrimitive.TryGetAttributeAccessorId(ACCESSOR_TANGENT, accessorId))
                    {
                        const Accessor& accessor = document.accessors.Get(accessorId);

                        const auto data = resourceReader.ReadBinaryData<float>(document, accessor);
                        vTangent = std::move(data);
                    }

                    std::vector <VertexPosUvNormTangent> geometry_data;
                    geometry_data.reserve(num_vert);

                    using vec3f = BruteForce::Math::Vec3Float;
                    using vec4f = BruteForce::Math::Vec4Float;
                    using vec2f = BruteForce::Math::Vec2Float;

                    for (size_t i = 0; i < num_vert; i ++)
                    {
                        size_t i3 = i * 3;
                        size_t i4 = i * 4;
                        size_t i2 = i * 2;
                        geometry_data.emplace_back( vec3f( vPositions[i3],vPositions[i3+1],-vPositions[i3+2])
                                                    ,vec2f(vUV[i2],vUV[i2 + 1])
                                                    ,vec3f(vNormals[i3],vNormals[i3 + 1],-vNormals[i3 + 2])
                                                    ,vec4f(vTangent[i4],vTangent[i4 + 1],-vTangent[i4 + 2], vTangent[i4 + 3])
                                                    );
                    }

                    const Accessor& accessor = document.accessors.Get(meshPrimitive.indicesAccessorId);
                    const auto indexes = resourceReader.ReadBinaryData<WORD>(document, accessor);
                    std::vector <WORD> indexes_data;
                    indexes_data.reserve(indexes.size());
                    for (size_t i = 0; i < indexes.size() / 3; i++)
                    {
                        size_t i3 = i * 3;
                        indexes_data.emplace_back(indexes[i3]);
                        indexes_data.emplace_back(indexes[i3+2]);
                        indexes_data.emplace_back(indexes[i3+1]);
                    }

                    CreateGeometry(device, geometry, reinterpret_cast<float*>(geometry_data.data()), geometry_data.size(), indexes_data.data(), indexes.size());
                }

                return true;
            }
        }
        void LoadGeometryGlb(Device& device, IndexedGeometry& geometry, std::filesystem::path path)
        {
            using namespace Microsoft::glTF;

            // Pass the absolute path, without the filename, to the stream reader
            auto streamReader = std::make_unique<StreamReader>(path.parent_path());

            std::filesystem::path pathFile = path.filename();
            std::filesystem::path pathFileExt = pathFile.extension();

            std::string manifest;

            /*auto MakePathExt = [](const std::string& ext)
            {
                return "." + ext;
            };*/

            std::unique_ptr<GLTFResourceReader> resourceReader;

            //// If the file has a '.gltf' extension then create a GLTFResourceReader
            //if (pathFileExt == MakePathExt(GLTF_EXTENSION))
            //{
            //    auto gltfStream = streamReader->GetInputStream(pathFile.u8string()); // Pass a UTF-8 encoded filename to GetInputString
            //    auto gltfResourceReader = std::make_unique<GLTFResourceReader>(std::move(streamReader));

            //    std::stringstream manifestStream;

            //    // Read the contents of the glTF file into a string using a std::stringstream
            //    manifestStream << gltfStream->rdbuf();
            //    manifest = manifestStream.str();

            //    resourceReader = std::move(gltfResourceReader);
            //}

            // If the file has a '.glb' extension then create a GLBResourceReader. This class derives
            // from GLTFResourceReader and adds support for reading manifests from a GLB container's
            // JSON chunk and resource data from the binary chunk.
            //if (pathFileExt == MakePathExt(GLB_EXTENSION))
            {
                auto glbStream = streamReader->GetInputStream(pathFile.u8string()); // Pass a UTF-8 encoded filename to GetInputString
                auto glbResourceReader = std::make_unique<GLBResourceReader>(std::move(streamReader), std::move(glbStream));

                manifest = glbResourceReader->GetJson(); // Get the manifest from the JSON chunk

                resourceReader = std::move(glbResourceReader);
            }

            if (!resourceReader)
            {
                throw std::runtime_error("Command line argument path filename extension must be .gltf or .glb");
            }

            Document document;

            try
            {
                document = Deserialize(manifest);
            }
            catch (const GLTFException& ex)
            {
                std::stringstream ss;

                ss << "Microsoft::glTF::Deserialize failed: ";
                ss << ex.what();

                throw std::runtime_error(ss.str());
            }

            GLTF::FillGeometryFromMesh(device, geometry, document, *resourceReader);

        }
    }
}