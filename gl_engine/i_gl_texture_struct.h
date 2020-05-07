#ifndef GL_TEXTURE_STRUCT
#define GL_TEXTURE_STRUCT
#include <GL/glew.h>
#include <string>
#include "glresourses.h"
struct IGlTextureStruct
{
    GLuint m_texture;

    IGlTextureStruct(const std::string & filename)
    {
        LoadTexture(filename,m_texture);
    }
    
    ~IGlTextureStruct()
    {
        glDeleteTextures(1,&m_texture);
    }
};
#endif