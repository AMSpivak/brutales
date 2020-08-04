#ifndef _GL_MATERIAL_H
#define _GL_MATERIAL_H
#include <GL/glew.h>
#include <string>
#include <memory>
#include "glresourses.h"
#include "i_gl_texture_struct.h"

namespace GameResource
{
    struct GlMaterial
    {
        std::shared_ptr<IGlTextureStruct> m_albedo_texture;
        std::shared_ptr<IGlTextureStruct> m_normal_height_texture;
        std::shared_ptr<IGlTextureStruct> m_roughness_metalness_texture;

        GlMaterial() = default;

        GlMaterial(   const std::string & albedo,
                            const std::string & normal_height,
                            const std::string & roughness_metalness//,
                            //const std::string & shader_prepass
                            //const std::map<const std::string,GLuint> &shader_map,
                            //const std::string & shader_deffered

                        );
        
        void Assign(GLuint shader, unsigned int start_texture, const char * albedo,const char * normal_height,const char *roughness_metalness);

        bool operator <(const GlMaterial& b) const
        {
            
            if (m_roughness_metalness_texture->m_texture == b.m_roughness_metalness_texture->m_texture)
            {
                if (m_normal_height_texture->m_texture == b.m_normal_height_texture->m_texture)
                {
                    return m_albedo_texture->m_texture < b.m_albedo_texture->m_texture;
                    
                }
                else
                {
                    return m_normal_height_texture->m_texture < b.m_normal_height_texture->m_texture;
                }
            }
            else
            {
                return m_roughness_metalness_texture->m_texture < b.m_roughness_metalness_texture->m_texture;
            }
        }
    };
}
#endif