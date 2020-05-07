#ifndef GL_MAP_BROWSER
#define GL_MAP_BROWSER
#include <memory>
#include "gl2d_item.h"
#include "i_gl_texture_struct.h"
namespace Gl2D
{
    class GlMapBrowser : public Gl2dItem
    {
        private:
        std::shared_ptr<IGlTextureStruct> m_map_texture;
        std::shared_ptr<IGlTextureStruct> m_blured_map_texture;
        std::shared_ptr<IGlTextureStruct> m_crosshair_texture;
        //std::shared_ptr<IGlTextureStruct> m_map_texture_blured;
        float m_zoom;
        float m_map_x;
        float m_map_y;
        float m_map_z;

        glm::vec4 m_main_image_corrector;

        GLuint m_shader;
        public:
        GlMapBrowser(float x,float y,float width, float height, float aspect_ratio, 
                                std::shared_ptr<IGlTextureStruct> map_texture,
                                std::shared_ptr<IGlTextureStruct> blured_map_texture,
                                std::shared_ptr<IGlTextureStruct> crosshair_texture,
                                GLuint shader):
                                                        Gl2dItem(x,y,width,height, aspect_ratio)
                                                        ,m_map_texture(map_texture)
                                                        ,m_blured_map_texture(blured_map_texture)
                                                        ,m_crosshair_texture(crosshair_texture)
                                                        ,m_shader(shader)
                                                        ,m_zoom(1.f)
                                                        ,m_map_x(0.f)
                                                        ,m_map_y(0.f)
                                                        ,m_map_z(0.52f)
                                                        ,m_main_image_corrector(1.0f,1.0f,1.0f,1.0f)
                                                        {}
        GlMapBrowser(float x,float y,float width, float height, float aspect_ratio, 
                                std::shared_ptr<IGlTextureStruct> map_texture,
                                GLuint shader):
                                                        Gl2dItem(x,y,width,height, aspect_ratio)
                                                        ,m_map_texture(map_texture)
                                                        ,m_shader(shader)
                                                        ,m_zoom(1.f)
                                                        ,m_map_x(0.f)
                                                        ,m_map_y(0.f)
                                                        ,m_map_z(0.0f)
                                                        ,m_main_image_corrector(1.0f,1.0f,1.0f,1.0f)
                                                       {}
        
        ~GlMapBrowser(){}
        void SetZoom(float value);
        void MoveMap(float x, float y);
        void SetFocus(float z);

        void Draw();

        const float GetZoom();        
        const float GetFocus();
        const float GetX();
        const float GetY();
        const glm::vec4 GetCorrector();
        void SetCorrector(const glm::vec4 && value);
    };

}

#endif