#include "gl_material.h"
#include "gl_resources_manager.h"

namespace GameResource
{
    GlMaterial::GlMaterial( const std::string & albedo,
                            const std::string & normal_height,
                            const std::string & roughness_metalness//,
                            //const std::map<const std::string,GLuint> &shader_map,
                            //const std::string & shader_deffered
                        )
    {
        auto pmanager = GetResourceManager();
        if(albedo!="")
        {
            m_albedo_texture = pmanager->m_texture_atlas.Assign(albedo);
        }
        if(normal_height!="")
        {
            m_normal_height_texture = pmanager->m_texture_atlas.Assign(normal_height);
        }
        if(roughness_metalness!="")
        {
            m_roughness_metalness_texture = pmanager->m_texture_atlas.Assign(roughness_metalness);
        }
    }

    void GlMaterial::Assign(GLuint shader,unsigned int start_texture, const char * albedo,const char * normal_height,const char *roughness_metalness)
    {
        GLenum texture =  GL_TEXTURE0 + start_texture;

        if(albedo) glUniform1i(glGetUniformLocation(shader, albedo), start_texture);
        glActiveTexture(texture);
        glBindTexture(GL_TEXTURE_2D, m_albedo_texture->m_texture);
        ++start_texture;

        texture =  GL_TEXTURE0 + start_texture;
        glUniform1i(glGetUniformLocation(shader, normal_height), start_texture);
        glActiveTexture(texture);
        glBindTexture(GL_TEXTURE_2D, m_normal_height_texture->m_texture);
        ++start_texture;

        texture =  GL_TEXTURE0 + start_texture;
        glUniform1i(glGetUniformLocation(shader, roughness_metalness), start_texture);
        glActiveTexture(texture);
        glBindTexture(GL_TEXTURE_2D, m_roughness_metalness_texture->m_texture);
    }

}