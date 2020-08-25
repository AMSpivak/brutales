#ifndef GL_MAP_HEIGHTMAP_H
#define GL_MAP_HEIGHTMAP_H
#include "gl_resources_manager.h"
#include <memory>

#include "glm/glm.hpp"
#include "gl_material.h"

namespace GameMap
{
    class HeightMap
    {
    public:
        HeightMap() :m_heightmap(nullptr),
                    m_map_size(10.0f, 10.0f, 10.0f),
                    quadVAO{0},
                    quadVBO{0},
                    quadIBO{0},
	                vert_count{ 0 }
        {}


        ~HeightMap();
        void LoadMap(std::string FileName);
        void SetParameters(float map_scaler,float height_scaler);
        float GetHeight(float x,float y);
        float GetMapScaler();
        float GetHeightScaler();
        void Draw(GLuint current_shader,const glm::vec3 &position,const GlScene::glCamera &camera, size_t simplify = 0);
        const glm::vec3 GetMapSize();
        void SetMapSize(const glm::vec3 &size);
        std::shared_ptr<IGlTextureStruct> m_heightmap_texture;
        std::shared_ptr<IGlTextureStruct> m_texture;

        private:
#define LOD_MAX 4
        float m_map_scaler;
        float m_height_scaler;
        float m_mesh_size[LOD_MAX];
        glm::vec3  m_map_size;       
        int m_height;
        int m_width;
        std::shared_ptr<GameResource::GlMaterial> m_material[3];
        unsigned char * m_heightmap;

        unsigned int quadVAO[LOD_MAX];
        unsigned int quadVBO[LOD_MAX];
        unsigned int quadIBO[LOD_MAX];
	    GLsizei vert_count[LOD_MAX];
        void CreateMap(size_t lod);
    };
    
}

#endif