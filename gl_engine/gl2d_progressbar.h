#ifndef GL2D_PROGRESSBAR_H
#define GL2D_PROGRESSBAR_H
#include <memory>
#include <functional>
#include "gl2d_item.h"
#include "i_gl_texture_struct.h"
namespace Gl2D
{
    class GlProgressbar : public Gl2dItem
    {
        using bar_func = std::function<float()>;
        private:
        std::shared_ptr<IGlTextureStruct> m_bar_texture;
        std::shared_ptr<IGlTextureStruct> m_fill_texture;
        //std::shared_ptr<IGlTextureStruct> m_map_texture_blurred;
        GLuint m_shader;
        bar_func m_value_function;
        public:
        GlProgressbar(float x,float y,float width, float height, float aspect_ratio,
                        std::shared_ptr<IGlTextureStruct> bar_texture,
                        std::shared_ptr<IGlTextureStruct> fill_texture,
                        GLuint shader, bar_func value_function):
                                                        Gl2dItem(x,y,width,height, aspect_ratio)
                                                        ,m_bar_texture(bar_texture)
                                                        ,m_fill_texture(fill_texture)
                                                        ,m_shader(shader)
                                                        ,m_value_function(value_function)
                                                        
                                                        {}
        ~GlProgressbar(){}
        void Draw();
    };

}

#endif