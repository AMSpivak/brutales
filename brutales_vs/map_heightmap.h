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
        HeightMap():m_heightmap(nullptr),
                    m_map_size(10.0f,10.0f,10.0f),
                    quadVAO(0),
                    quadVBO(0),
                    quadIBO(0),
	                vert_count(0){}

        ~HeightMap();
        void LoadMap(std::string FileName);
        void SetParameters(float map_scaler,float height_scaler);
        float GetHeight(float x,float y);
        float GetMapScaler();
        float GetHeightScaler();
        void Draw(GLuint current_shader,const glm::vec3 &position,const GlScene::glCamera &camera, size_t simplify = 1);
        const glm::vec3 GetMapSize();
        void SetMapSize(const glm::vec3 &size);
        std::shared_ptr<IGlTextureStruct> m_heightmap_texture;
        std::shared_ptr<IGlTextureStruct> m_texture;

        private:
        float m_map_scaler;
        float m_height_scaler;
        float m_mesh_size;
        glm::vec3  m_map_size;       
        int m_height;
        int m_width;
        std::shared_ptr<GameResource::GlMaterial> m_material[3];
        unsigned char * m_heightmap;

        unsigned int quadVAO;
        unsigned int quadVBO;
        unsigned int quadIBO;
	    GLsizei vert_count;
        void CreateMap();
    };
    
}

#endif