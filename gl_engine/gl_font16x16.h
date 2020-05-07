#ifndef GL_TEXT16x16
#define GL_TEXT16x16
#include "i_gl_font.h"
class GlText16x16 : public IGlText
{
    public:
    GlText16x16(const std::string & filename, GLAtlas<IGlTextureStruct> &m_texture_atlas,float width, float height);
    
    
    ~GlText16x16()
    {
    }

    virtual void DrawString(const std::string &value,float x,float y, GLuint shader);
    virtual float GetStringLength(const std::string &value);

};
#endif