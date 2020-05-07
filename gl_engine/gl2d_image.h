#ifndef GL_MAP_BROWSER
#define GL_MAP_BROWSER
#include <memory>
#include "gl2d_item.h"
#include "i_gl_texture_struct.h"
namespace Gl2D
{
    class GlImage : public Gl2dItem
    {
        using sp_texture = std::shared_ptr<IGlTextureStruct>;
        private:
        sp_texture m_texture;
        GLuint m_shader;
        public:
        GlImage(float x,float y,float width, float height, float aspect_ratio, sp_texture texture,GLuint shader):
                                                        Gl2dItem(x,y,width,height, aspect_ratio), m_texture(texture)
                                                        ,m_shader(shader)

                                                         {}
        ~GlImage(){}

        void SetImage(sp_texture image);

        void Draw();
    };

}

#endif