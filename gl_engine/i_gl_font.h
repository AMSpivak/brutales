#ifndef GL_TEXT
#define GL_TEXT
#include <GL/glew.h>
#include <string>
#include "glresourses.h"
#include "gl_atlas.h"
#include "i_gl_texture_struct.h"
class IGlText
{
    protected:
    float m_width;
    float m_height;
	std::shared_ptr<IGlTextureStruct> texture;    
    public:
    IGlText(const std::string & filename, GLAtlas<IGlTextureStruct> &m_texture_atlas,float width, float height)
    {
        //LoadTexture(filename,m_texture);
        SetTextSize(width,height);
    }
    
    
    ~IGlText()
    {
    }

    void SetTextSize(float width, float height)
    {
        m_width = width;
        m_height = height;
    }
    virtual void DrawString(const std::string &value,float x,float y, GLuint shader) = 0;
    virtual float GetStringLength(const std::string &value) = 0;
};
#endif