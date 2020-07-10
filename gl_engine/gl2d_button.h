#ifndef GL2D_BUTTON
#define GL2D_BUTTON
#include <memory>
#include <functional>
#include "gl2d_item.h"
#include "i_gl_texture_struct.h"
#include "gl_font16x16.h"
namespace Gl2D
{
    class GlButton : public Gl2dItem
    {
        using sp_texture = std::shared_ptr<IGlTextureStruct>;
        private:
        sp_texture m_texture;
        sp_texture m_texture_active;
        std::shared_ptr<IGlText> m_font;
        std::string m_text;
        GLuint m_shader;
        float m_active_mul;
        Action m_action;
        public:
        GlButton(float aspect_ratio): Gl2dItem(aspect_ratio) {}
        GlButton(float x,float y,float width, float height, float aspect_ratio,
                 sp_texture texture,sp_texture texture_active,
                  std::shared_ptr<IGlText> font, const std::string &text,
                  GLuint shader,
                  Action action):
                                                        Gl2dItem(x,y,width,height, aspect_ratio), m_texture(texture), m_texture_active(texture_active)
                                                        ,m_font(font),m_text(text)
                                                        ,m_shader(shader),m_active_mul(1.0f)
                                                        ,m_action(action)

                                                         {}
        ~GlButton(){}
        void SetFont(std::shared_ptr<IGlText> font) {m_font = font;}
        void SetAction(Action action) {m_action = action;}
        void SetImage(sp_texture image) { m_texture = image; }
        void SetActiveSizer(float value) { m_active_mul = value; }
        void Draw();
        std::string Load(const std::vector<std::string>& lines, Interface2D& interface);
        
        std::weak_ptr<Gl2dItem> ProcessInput(Inputs::InputCommands input) override;
    };

}

#endif